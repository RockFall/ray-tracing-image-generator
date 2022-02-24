#pragma once

#include <glm/glm.hpp>

#include "SceneData.h"

class RayTracing {
public:
	RayTracing() {}
	~RayTracing() {}
	// Returns the color of a pixel given the parameters, applying ray tracing
	glm::vec3 CalculateRay(const SceneData& scene_data, const glm::vec3& ray_origin, const glm::vec3& ray_direction, 
						   int depth,
						   SurfaceType ignored_obj = SurfaceType::None, int ignored_idx = -1);
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

	// Calculate which object the given ray intersect closely to
	IntersectionReturn CalculateIntersections(const SceneData& scene_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction, 
											  const glm::vec3& end_pos = glm::vec3(std::numeric_limits<float>::max()), 
											  SurfaceType ignored_obj = SurfaceType::None, int ignored_idx = -1);
	// Returns 
	// 	        Value of 't'
	//          TRUE if is inside an object
	std::pair<float, bool> SphereIntersections(const SphereSurfaceData& sphere_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction, const glm::vec3& end_pos = glm::vec3(std::numeric_limits<float>::max()));
	// Returns 
	// 	        Value of 't'
	//          NORMAL
	std::pair<float, glm::vec3> PolyhedronIntersections(const PolyhedronSurfaceData& poly_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction, const glm::vec3& end_pos = glm::vec3(std::numeric_limits<float>::max()));

	glm::vec3 ReflectionDirection(const glm::vec3& ray_direction, const glm::vec3& normal);
	std::pair<bool, glm::vec3> TransmissionDirection(float refr_rate, const glm::vec3& ray_direction, const glm::vec3& normal);

};