#include "RayTracing.h"

RayTracing::IntersectionReturn RayTracing::CalculateIntersections(const SceneData& scene_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction)
{
	IntersectionReturn closest = IntersectionReturn();
	int index = 0;

	for each (SphereSurfaceData sphere in scene_data.spheres)
	{
		std::pair<float, bool> result = SphereIntersections(sphere, ray_origin, ray_direction);
		float t = result.first;
		if (t < closest.t) {
			closest.t = t;
			closest.type = SurfaceType::Sphere;
			closest.index = index;
			closest.is_inside = result.second;
		}
		++index;
	}

	index = 0;

	for each (PolyhedronSurfaceData polyhedron in scene_data.polyhedrons) 
	{
		std::pair<float, glm::vec3> result = PolyhedronIntersections(polyhedron, ray_origin, ray_direction);
		float t = result.first;
		if (t < closest.t) {
			closest.t = t;
			closest.type = SurfaceType::Polyhedron;
			closest.index = index;
			closest.normal = result.second;
		}
		++index;
		++index;
	}

	if (closest.type != SurfaceType::None) {
		closest.intersec_pos = ray_origin + closest.t * ray_direction;
		if (closest.type == SurfaceType::Sphere) {
			closest.intersec_color = scene_data.calculate_color(closest.intersec_pos, scene_data.spheres[closest.index].pigment_id);

		}
	}

	return closest;
	
}

std::pair<float, bool> RayTracing::SphereIntersections(const SphereSurfaceData& sphere_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction)
{
	glm::vec3 sphere_to_origin = ray_origin - sphere_data.pos;

	// a = V_x^2 + V_y^2 + V_z^2
	float a = glm::dot(ray_direction, ray_direction);
	// b = 2 (V_x*P_x + V_y*P_y + V_z*P_z)
	float b = 2 * glm::dot(ray_direction, sphere_to_origin);
	// c = (P_x^2 + P_y^2 + P_z^2 - 1)
	float c = glm::dot(sphere_to_origin, sphere_to_origin) - glm::pow(sphere_data.radius, 2);

	float delta = glm::pow(b, 2) - (4 * a * c);

	if (delta < 0) {
		// Not colliding
		return std::make_pair(false, -1);
	}

	// (-b +- sqrt(delta)) / 2a
	delta = glm::sqrt(delta);
	float r1 = (-b - delta) / (2 * a);
	float r2 = (-b + delta) / (2 * a);

	if (r1 > 0) {
		return std::make_pair(false, r1);
	}

	if (r2 > 0) {
		return std::make_pair(true, r2);
	}

	return std::make_pair(false, -1);
	
}

std::pair<float, glm::vec3> RayTracing::PolyhedronIntersections(const PolyhedronSurfaceData& poly_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction)
{
}
