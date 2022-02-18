#include <iostream>

#include <memory>
#include <string>
#include<sstream>
#include <iostream>
#include <fstream>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Renderer.h"
#include "SceneData.h"

void InputInterface(std::string& input_file, std::string& output_file, int& WIDTH, int& HEIGHT) {
	std::cout << "Insira os nomes dos arquivos de entrada e sa�da ( i.e. 'in.txt out.ppm' )" << std::endl;
	std::cout << "> Opcionalmente pode tamb�m ser inserido a largura e altura da imagem ( i.e. 'in.txt out.ppm 1000 1000' )" << std::endl;
	std::string input;
	std::getline(std::cin, input);
	std::istringstream ss(input);
	int count = 0;
	while (std::getline(ss, input, ' ')) {
		if (count == 0)
			input_file = input;
		if (count == 1)
			output_file = input;
		if (count == 2)
			WIDTH = std::stoi(input);
		if (count == 3)
			HEIGHT = std::stoi(input);
		++count;
	}
}

SceneData GetSceneData(SceneData& scene, const std::string input_file) {

	std::fstream file;
	file.open(input_file);

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
	camera.up_vector = { x, y, z };
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
		light.dist_mitigation = z;
		light.dist_sqr_mitigation = y;

		scene.lights.push_back(light);
	}

	// Pigments data
	file >> count;
	for (int i = 0; i < count; ++i) {
		file >> type;
		if (type == "solid") {
			SolidPigmentData solid = SolidPigmentData();
			file >> x >> y >> z;
			solid.rgb = { x, y, z };
			scene.solid_pigments.push_back(solid);
		}
		else if (type == "texmap") {
			TexmapPigmentData texmap = TexmapPigmentData();
			file >> aux;
			texmap.file_name = aux;
			file >> x >> y >> z >> w;
			texmap.tex_map_x = glm::vec4(x, y, z, w);
			file >> x >> y >> z >> w;
			texmap.tex_map_y = glm::vec4(x, y, z, w);
			scene.texmap_pigments.push_back(texmap);
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

	return scene;
}

int main() {
	// Get input data
	std::string input_file, output_file;
	int WIDTH = 800, HEIGHT = 600;
	//InputInterface(input_file, output_file);
	input_file = "input.txt";
	output_file = "out.ppm";

	SceneData scene = SceneData();
	scene = GetSceneData(scene, input_file);
	
	// Must be 402rainbow1.ppm
	std::cout << scene.checker_pigments[0].size;
	std::cout << scene.spheres[1].finish_id;
	std::cout << scene.texmap_pigments[0].file_name;
	
	// Set-up Window
	std::unique_ptr<Window> window = std::make_unique<Window>(WindowConfigs("file_name", WIDTH, HEIGHT));

	// Set-up Renderer
	std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
	renderer->Init();
	renderer->SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });

	// Set-up Camera
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(
		scene.camera.pos, 
		scene.camera.target, 
		scene.camera.up_vector, 
		scene.camera.fov,
		WIDTH,
		HEIGHT);

	while (true) {
		// Clear scene
		renderer->Clear();

		// Render all objects
		renderer->BeginScene(*camera);

		renderer->EndScene();

		// Poll events and Swap Buffers
		window->OnUpdate();
	}

	return 0;
}
