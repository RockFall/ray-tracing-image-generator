#pragma once

#include <glm/glm.hpp>

#include "SceneData.h"

class RayTracing {
	RayTracing() {}
	~RayTracing() {}

	void Calculate(const SceneData& scene_data, const glm::vec3& ray_origin, const glm::vec3& ray_direction);
private:
	struct IntersectionReturn {
		SurfaceType type			= SurfaceType::None;
		int			index			= -1;
		float		t				= std::numeric_limits<float>::max();
		bool		is_inside		= false;
		glm::vec3	normal			= glm::vec3(0.0f);
		glm::vec3	intersec_pos	= glm::vec3(0.0f);
		glm::vec3	intersec_color  = glm::vec3(0.0f);
	};

	IntersectionReturn CalculateIntersections(const SceneData& scene_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction);
	// Returns TRUE if is inside an object
	std::pair<float, bool> SphereIntersections(const SphereSurfaceData& sphere_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction);
	std::pair<float, glm::vec3> PolyhedronIntersections(const PolyhedronSurfaceData& poly_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction);
};