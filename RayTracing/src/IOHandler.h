#pragma once

#include "SceneData.h"

class IOHandler {
public:
	~IOHandler() {}

	static bool GetSceneData(SceneData& scene, const std::string input_file) {

		std::fstream file;
		file.open(input_file);
		if (!file.is_open()) {
			std::cout << "Error while opening the file " << input_file << " (probably not found)" << std::endl;
			return false;
		}

		std::string input;
		std::string type, aux;
		float x = 0, y = 0, z = 0, w = 0;
		float aux_f = 0;
		int count = 0;

		// Camera Data
		CameraData camera = CameraData();
		file >> x >> y >> z;
		camera.pos = { x, y, z };
		file >> x >> y >> z;
		camera.target = { x, y, z };
		file >> x >> y >> z;
		camera.up_vector = glm::normalize(glm::vec3(x, y, z));
		file >> aux_f;
		camera.fov = aux_f;
		scene.camera = camera;

		// Light Data
		file >> count;
		for (int i = 0; i < count; ++i) {
			LightData light = LightData();
			file >> x >> y >> z;
			light.pos = { x, y, z };
			file >> x >> y >> z;
			light.rgb = { x, y, z };
			file >> x >> y >> z;
			light.const_mitigation = x;
			light.dist_mitigation = y;
			light.dist_sqr_mitigation = z;

			scene.lights.push_back(light);
		}

		// Pigments data
		file >> count;
		int pigm_index = 0;
		for (int i = 0; i < count; ++i) {
			file >> type;
			if (type == "solid") {
				SolidPigmentData solid = SolidPigmentData();
				file >> x >> y >> z;
				solid.rgb = { x, y, z };
				scene.solid_pigments.push_back(solid);
				scene.pigment_map[pigm_index] = { PigmentType::Solid, scene.solid_pigments.size() - 1 };
				++pigm_index;
			}
			else if (type == "texmap") {
				TexmapPigmentData texmap = TexmapPigmentData();
				file >> aux;
				texmap.texture = Texture2D(aux);
				file >> x >> y >> z >> w;
				texmap.tex_map_x = glm::vec4(x, y, z, w);
				file >> x >> y >> z >> w;
				texmap.tex_map_y = glm::vec4(x, y, z, w);
				scene.texmap_pigments.push_back(texmap);
				scene.pigment_map[pigm_index] = { PigmentType::Texmap, scene.texmap_pigments.size() - 1 };
				++pigm_index;
			}
			else if (type == "checker") {
				CheckerPigmentData checker = CheckerPigmentData();
				file >> x >> y >> z;
				checker.rgb1 = { x, y, z };
				file >> x >> y >> z;
				checker.rgb2 = { x, y, z };
				file >> aux_f;
				checker.size = aux_f;
				scene.checker_pigments.push_back(checker);
				scene.pigment_map[pigm_index] = { PigmentType::Checker, scene.checker_pigments.size() - 1 };
				++pigm_index;
			}
			else {
				std::cout << "Invalid Pigment type given: " << type << std::endl;
				assert(false);
			}
		}

		// Finishes Data
		file >> count;
		for (int i = 0; i < count; ++i) {
			SurfaceFinishData finish = SurfaceFinishData();
			file >> x >> y >> z >> w;
			finish.ka = x;
			finish.kd = y;
			finish.ks = z;
			finish.a = w;
			file >> x >> y >> z;
			finish.kr = x;
			finish.kt = y;
			finish.ior = z;
			scene.finishes.push_back(finish);
		}

		// Surfaces Data
		file >> count;
		for (int i = 0; i < count; ++i) {
			file >> x >> y;
			file >> type;
			if (type == "sphere") {
				SphereSurfaceData sphere = SphereSurfaceData();
				sphere.pigment_id = int(x);
				sphere.finish_id = int(y);
				file >> x >> y >> z >> aux_f;
				sphere.pos = glm::vec3(x, y, z);
				sphere.radius = aux_f;
				scene.spheres.push_back(sphere);
			}
			else if (type == "polyhedron") {
				PolyhedronSurfaceData polyhedron = PolyhedronSurfaceData();
				polyhedron.pigment_id = int(x);
				polyhedron.finish_id = int(y);
				file >> aux_f;
				for (int i = 0; i < aux_f; i++)
				{
					PolyhedronFaceData face = PolyhedronFaceData();
					file >> x >> y >> z >> w;
					face.plane_equation = glm::vec4(x, y, z, w);
					polyhedron.faces.push_back(face);
				}
				scene.polyhedrons.push_back(polyhedron);
			}
			else {
				std::cout << "Invalid Surface type given: " << type << std::endl;
				assert(false);
			}
		}

		return true;
	}

	static void WriteFile(SceneData scene_data, std::shared_ptr<Camera> camera, std::string output_file_name, int WIDTH, int HEIGHT, int depth) {
		std::ofstream file;
		file.open(output_file_name, std::ios::out);
		if (!file.is_open()) {
			std::cout << "Could not open or create the file '" << output_file_name << "'" << std::endl;
		}

		file << "P3" << std::endl;
		file << WIDTH << " " << HEIGHT << std::endl;
		file << "255" << std::endl;

		float screen_height = 2.0f * glm::tan(glm::radians(scene_data.camera.fov / 2.0f)) * glm::l2Norm(scene_data.camera.target - scene_data.camera.pos);
		float screen_width = (screen_height * WIDTH) / HEIGHT;

		glm::vec3 pixel_right_dir = ((screen_width) / WIDTH) * (camera->get_transform().get_right_direction());
		glm::vec3 pixel_down_dir = (screen_height / HEIGHT) * (camera->get_transform().get_up_direction() * -1.0f);

		glm::vec3 top_left = camera->get_target() - (camera->get_transform().get_right_direction() * (screen_width / 2.0f)) + (camera->get_transform().get_up_direction() * (screen_height / 2.0f));

		RayTracing ray_tracer = RayTracing();

		std::cout << "\n- Starting Ray tracing -" << std::endl;
		float percentage;
		for (int h = 0; h < HEIGHT; h++)
		{
			for (int w = 0; w < WIDTH; w++)
			{
				if (h == HEIGHT / 4.0f && w == 0) {
					std::cout << "25%" << std::endl;
				}
				if (h == HEIGHT / 2.0f && w == 0) {
					std::cout << "50%" << std::endl;
				}
				if (h == (HEIGHT / 4.0f) * 3 && w == 0) {
					std::cout << "75%" << std::endl;
				}

				glm::vec3 pos = glm::vec3(top_left);
				pos += float(w) * pixel_right_dir;
				pos += float(h) * pixel_down_dir;
				glm::vec3 camera_to_pixel_dir = glm::normalize(pos - camera->get_transform().get_position());

				glm::vec3 pixel_color = ray_tracer.CalculateRay(scene_data, camera->get_transform().get_position(), camera_to_pixel_dir, depth);
				file << int(pixel_color.x * 255) << " " << int(pixel_color.y * 255) << " " << int(pixel_color.z * 255) << std::endl;
			}
		}

		std::cout << "100%" << std::endl;

		file.close();
	}

protected:
	IOHandler() {}
};