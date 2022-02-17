#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

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
	std::string file_name;
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
	CameraData camera;
	// Light
	LightData ambient_light;
	std::vector<LightData> lights;
	// Pigments
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
	glm::vec2 get_2d_texture_coord(glm::vec3 coord_3d, TexmapPigmentData texmap) { 
		return glm::vec2(glm::dot(texmap.tex_map_x, glm::vec4(coord_3d, 1.0f)), glm::dot(texmap.tex_map_y, glm::vec4(coord_3d, 1.0f)));
	}

};