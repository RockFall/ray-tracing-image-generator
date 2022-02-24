#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <assert.h>

#include <glm/glm.hpp>

#include "Texture2D.h"

enum class SurfaceType {
	None,
	Sphere,
	Polyhedron
};

enum class PigmentType {
	None,
	Solid,
	Checker,
	Texmap
};

struct CameraData {
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 up_vector;
	float fov;
};

struct LightData {
	glm::vec3 pos;
	glm::vec3 rgb;
	float const_mitigation;
	float dist_mitigation;
	float dist_sqr_mitigation;
};

struct SolidPigmentData {
	glm::vec3 rgb;
};

struct TexmapPigmentData {
	Texture2D texture = Texture2D();
	glm::vec4 tex_map_x;
	glm::vec4 tex_map_y;
};

struct CheckerPigmentData {
	glm::vec3 rgb1;
	glm::vec3 rgb2;
	float size;
};

struct SurfaceFinishData {
	float ka, kd, ks, a, kr, kt, ior;
};

struct SphereSurfaceData {
	int pigment_id;
	int finish_id;
	glm::vec3 pos;
	float radius;
};

struct PolyhedronFaceData {
	glm::vec4 plane_equation;
};

struct PolyhedronSurfaceData {
	int pigment_id;
	int finish_id;
	std::vector<PolyhedronFaceData> faces;
};



class SceneData {
public:
	SceneData() {};
	~SceneData() {};

	// Camera
	CameraData camera = CameraData();
	// Light
	LightData ambient_light = LightData();
	std::vector<LightData> lights;
	// Pigments
	std::unordered_map<int, std::pair<PigmentType, int>> pigment_map;
	std::vector<SolidPigmentData> solid_pigments;
	std::vector<TexmapPigmentData> texmap_pigments;
	std::vector<CheckerPigmentData> checker_pigments;
	// Finishes
	std::vector<SurfaceFinishData> finishes;
	// Surfaces
	std::vector<SphereSurfaceData> spheres;
	std::vector<PolyhedronSurfaceData> polyhedrons;

	float get_light_count() { return lights.size() + 1; }
	float get_total_pigments_count() { return solid_pigments.size() + texmap_pigments.size() + checker_pigments.size(); }
	glm::vec2 get_2d_texture_coord(glm::vec3 coord_3d, TexmapPigmentData texmap) const { 
		return glm::vec2(glm::dot(texmap.tex_map_x, glm::vec4(coord_3d, 1.0f)), glm::dot(texmap.tex_map_y, glm::vec4(coord_3d, 1.0f)));
	}

	glm::vec3 calculate_color(const glm::vec3& pos, int pigment_id) const {
		auto type_id = pigment_map.find(pigment_id)->second;


		switch (type_id.first)
		{
		case PigmentType::Solid:
		{
			return solid_pigments[type_id.second].rgb;
		}
		break;
		case PigmentType::Checker:
		{
			float side_size = checker_pigments[type_id.second].size;
			int pattern = std::floor(pos.x / side_size) + std::floor(pos.y / side_size) + std::floor(pos.z / side_size);
			pattern = pattern % 2;

			if (!pattern)
				return checker_pigments[type_id.second].rgb1;
			else
				return checker_pigments[type_id.second].rgb2;
		}
		break;
		case PigmentType::Texmap:
		{
			glm::vec2 coords = get_2d_texture_coord(pos, texmap_pigments[type_id.second]);
			//int i = (int)(coords.y * texmap_pigments[type_id.second].texture.get_height()) % texmap_pigments[type_id.second].texture.get_height();
			int i = mod(int(coords.y * texmap_pigments[type_id.second].texture.get_height()), texmap_pigments[type_id.second].texture.get_height());
			//int j = (int)(coords.x * texmap_pigments[type_id.second].texture.get_width()) % texmap_pigments[type_id.second].texture.get_width();
			int j = mod(int(coords.x * texmap_pigments[type_id.second].texture.get_width()), texmap_pigments[type_id.second].texture.get_width());
			if (i < 0)
				i += texmap_pigments[type_id.second].texture.get_height();
			if (j < 0)
				j += texmap_pigments[type_id.second].texture.get_width();

			return texmap_pigments[type_id.second].texture.get_pixel(j, i);
		}
		break;
		case PigmentType::None:
		{
			return glm::vec3(0.0f);
		}
		break;
		default:
		{
			// Invalid PigmentType passed
			assert(false);
			return glm::vec3(0.0f);
		}
			break;
		}
	}

	int mod(int a, int b) const
	{
		if (b < 0) //you can check for b == 0 separately and do what you want
			return -mod(-a, -b);
		int ret = a % b;
		if (ret < 0)
			ret += b;
		return ret;
	}
};