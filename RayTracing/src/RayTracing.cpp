#include "RayTracing.h"
#include <glm/gtx/norm.hpp>

#define EPSILON std::numeric_limits<float>::epsilon()
#define MAX_FLOAT std::numeric_limits<float>::max()

glm::vec3 RayTracing::CalculateRay(const SceneData& scene_data, const glm::vec3& ray_origin, const glm::vec3& ray_direction, int depth, SurfaceType ignored_obj, int ignored_idx)
{
	// Get the intersection of this ray
	IntersectionReturn intersection = CalculateIntersections(scene_data, ray_origin, ray_direction, glm::vec3(MAX_FLOAT), ignored_obj, ignored_idx);
	if (intersection.type == SurfaceType::None) {
		// If not intersecting anything, return ambient color
		return scene_data.lights[0].rgb;
	}

	// Get finish of intersected Object
	SurfaceFinishData intersected_obj_finish = SurfaceFinishData();
	if (intersection.type == SurfaceType::Sphere) {
		intersected_obj_finish = scene_data.finishes[scene_data.spheres[intersection.index].finish_id];
	}
	else if (intersection.type == SurfaceType::Polyhedron) {
		intersected_obj_finish = scene_data.finishes[scene_data.polyhedrons[intersection.index].finish_id];
	}

	// ------- LIGHTNING -------
	glm::vec3 ambient_color = scene_data.lights[0].rgb * intersected_obj_finish.ka;
	glm::vec3 diffuse_color = glm::vec3(0);
	glm::vec3 specular_color = glm::vec3(0);

	// Iterate through lights
	auto it = std::begin(scene_data.lights);
	// Skip first light (ambient)
	++it;
	for (auto end=std::end(scene_data.lights); it != end; ++it)
	{
		LightData light = *it;

		glm::vec3 light_dir = glm::normalize(light.pos - intersection.intersec_pos);
		float dist = glm::l2Norm(light.pos - intersection.intersec_pos);
		float mitigation = 1.0f / (light.const_mitigation + dist * light.dist_mitigation + pow(dist, 2) * light.dist_sqr_mitigation);

		// Check if there is something between this point and current light
		IntersectionReturn barrier = CalculateIntersections(scene_data, intersection.intersec_pos, light_dir, light.pos, intersection.type, intersection.index);
		if (barrier.type != SurfaceType::None) {
			continue;
		}

		float cos_diffuse = glm::dot(light_dir, intersection.normal);
		if (cos_diffuse < EPSILON)
			cos_diffuse = 0;
		diffuse_color += (light.rgb * cos_diffuse * intersected_obj_finish.kd * mitigation);

		float cos_specular = glm::dot(glm::normalize(light_dir + (ray_direction * -1.0f)), glm::normalize(intersection.normal));
		if (cos_specular < EPSILON)
			cos_specular = 0;

		cos_specular = glm::pow(cos_specular, intersected_obj_finish.a);
		specular_color += (light.rgb * cos_specular * intersected_obj_finish.ks * mitigation);
	}

	// -------- REFLECTION AND TRANSMISSION --------
	glm::vec3 reflect_color = glm::vec3(0);
	glm::vec3 transmission_color = glm::vec3(0);

	if (depth > 0) {
		glm::vec3 reflect_dir = ReflectionDirection(ray_direction, intersection.normal);
		reflect_color += (CalculateRay(scene_data, intersection.intersec_pos, reflect_dir, depth - 1, intersection.type, intersection.index) * intersected_obj_finish.kr);

		float refraction_rate;
		if (intersected_obj_finish.ior < EPSILON)
			refraction_rate = 1;
		else
			refraction_rate = 1 / intersected_obj_finish.ior;

		if (intersection.is_inside) {
			refraction_rate = intersected_obj_finish.ior;
		}
		
		std::pair<bool, glm::vec3> result = TransmissionDirection(refraction_rate, ray_direction, intersection.normal);
		bool transmited = result.first;
		glm::vec3 transmission_dir = result.second;
		if (transmited) {
			transmission_color += (CalculateRay(scene_data, intersection.intersec_pos, transmission_dir, depth - 1, intersection.type, intersection.index) * intersected_obj_finish.kt);
		}
	}
	glm::vec3 final_color = intersection.intersec_color * (ambient_color + diffuse_color) + specular_color + transmission_color + reflect_color;
	
	if (final_color.y > 1) {
		int a = 3;
	}
	
	if (final_color.x > 1) {
		final_color.x = 1;
	} 
	 
	if (final_color.y > 1) {
		final_color.y = 1;
	}
	
	if(final_color.z > 1) {
		final_color.z = 1;
	}

	

	return final_color;
}

RayTracing::IntersectionReturn RayTracing::CalculateIntersections(const SceneData& scene_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction, const glm::vec3& end_pos, SurfaceType ignored_obj, int ignored_idx)
{
	IntersectionReturn closest = IntersectionReturn();
	int index = 0;

	for each (SphereSurfaceData sphere in scene_data.spheres)
	{
		if (ignored_obj == SurfaceType::Sphere && ignored_idx == index) {
			++index;
			continue;
		}

		std::pair<float, bool> result = SphereIntersections(sphere, ray_origin, ray_direction, end_pos);
		float t = result.first;
		if (t >= EPSILON && t < closest.t) {
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
		if (ignored_obj == SurfaceType::Polyhedron && ignored_idx == index) {
			++index;
			continue;
		}

		std::pair<float, glm::vec3> result = PolyhedronIntersections(polyhedron, ray_origin, ray_direction, end_pos);
		float t = result.first;
		if (t >= EPSILON && t < closest.t) {
			closest.t = t;
			closest.type = SurfaceType::Polyhedron;
			closest.index = index;
			closest.normal = result.second;
		}
		++index;
	}

	if (closest.is_inside && closest.type == SurfaceType::Polyhedron)
		closest.is_inside = false;

	if (closest.type != SurfaceType::None) {
		// Intersection in 'ray origin' + 't'
		closest.intersec_pos = ray_origin + closest.t * ray_direction;

		if (closest.type == SurfaceType::Sphere) {
			closest.intersec_color = scene_data.calculate_color(closest.intersec_pos, scene_data.spheres[closest.index].pigment_id);
			closest.normal = glm::normalize(closest.intersec_pos - scene_data.spheres[closest.index].pos);
			if (closest.is_inside) {
				closest.normal *= -1.0f;
			}
		}
		else if (closest.type == SurfaceType::Polyhedron) {
			closest.intersec_color = scene_data.calculate_color(closest.intersec_pos, scene_data.polyhedrons[closest.index].pigment_id);
		}
	}

	return closest;
	
}

std::pair<float, bool> RayTracing::SphereIntersections(const SphereSurfaceData& sphere_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction, const glm::vec3& end_pos)
{
	glm::vec3 sphere_to_origin = ray_origin - sphere_data.pos;

	// a = V_x^2 + V_y^2 + V_z^2
	float a = glm::dot(ray_direction, ray_direction);
	// b = 2 (V_x*P_x + V_y*P_y + V_z*P_z)
	float b = float(2) * glm::dot(ray_direction, sphere_to_origin);
	// c = (P_x^2 + P_y^2 + P_z^2 - 1)
	float c = glm::dot(sphere_to_origin, sphere_to_origin) - float(glm::pow(sphere_data.radius, 2));

	float delta = float(glm::pow(b, 2) - (4 * a * c));

	if (delta < EPSILON) {
		// Not colliding
		return std::make_pair(-1, false);
	}

	if (sphere_data.pigment_id == 1) {
		int a = 3;
	}

	// (-b +- sqrt(delta)) / 2a
	delta = glm::sqrt(delta);
	float t1 = (-b - delta) / (2 * a);
	float t2 = (-b + delta) / (2 * a);

	float max_t = MAX_FLOAT;

	if (end_pos[0] != MAX_FLOAT) {
		if (ray_direction[0] < EPSILON) {
			max_t = (end_pos[0] - ray_origin[0]);
		}
		else {
			max_t = (end_pos[0] - ray_origin[0]) / ray_direction[0];
		}
	}

	if (t1 > 0 && t1 < max_t) {
		return std::make_pair(t1, false);
	}

	if (t2 > 0 && t2 < max_t) {
		return std::make_pair(t2, true);
	}

	return std::make_pair(- 1, false);
	
}

std::pair<float, glm::vec3> RayTracing::PolyhedronIntersections(const PolyhedronSurfaceData& poly_data, const glm::vec3& ray_origin, const glm::vec3 ray_direction, const glm::vec3& end_pos)
{
	float t0 = 0;
	glm::vec3 t0_equation = glm::vec3(0);
	glm::vec3 t1_equation = glm::vec3(0);
	float t1 = MAX_FLOAT;
	glm::vec3 normal = glm::vec3(0);

	for each (PolyhedronFaceData face in poly_data.faces)
	{
		glm::vec3 p0 = ray_origin;
		glm::vec3 equation = glm::vec3(face.plane_equation.x, face.plane_equation.y, face.plane_equation.z);

		float dot_equation = glm::dot(ray_direction, equation);
		float val = glm::dot(p0, equation) + face.plane_equation.w;

		if (dot_equation <= EPSILON && dot_equation >= -EPSILON) {
			if (val > EPSILON) {
				t1 = -1;
			}
		}
		else if (dot_equation > EPSILON) {
			float t = -val / dot_equation;
			if (t < t1) {
				t1 = t;
				t1_equation = equation;
			}
		}
		else if (dot_equation < -EPSILON) {
			float t = -val / dot_equation;
			if (t > t0) {
				t0 = t;
				t0_equation = equation;
			}
		}
	}

	float max_t = MAX_FLOAT;

	if (end_pos[0] != MAX_FLOAT) {
		if (ray_direction[0] < EPSILON) {
			max_t = (end_pos[0] - ray_origin[0]);
		}
		else {
			max_t = (end_pos[0] - ray_origin[0]) / ray_direction[0];
		}
	}

	if (t1 < t0) {
		return std::make_pair(-1, normal);
	}

	if (abs(t0) <= EPSILON && t1 >= t0 && t1 < MAX_FLOAT) {
		normal = glm::normalize(t1_equation * -1.0f);
		if (t1 < max_t) {
			return std::make_pair(t1, normal);
		}
		else {
			return std::make_pair(-1, normal);
		}
		
	}

	if (t0 > EPSILON && t1 >= t0) {
		normal = glm::normalize(t0_equation);
		if (t1 < max_t) {
			return std::make_pair(t0, normal);
		}
		else {
			return std::make_pair(-1, normal);
		}
	}

	return std::make_pair(-1.0f, normal);
}

glm::vec3 RayTracing::ReflectionDirection(const glm::vec3& ray_direction, const glm::vec3& normal)
{
	float c = glm::dot(-1.0f * ray_direction, normal);
	return glm::normalize(ray_direction + (2.0f * normal * c));
}

std::pair<bool, glm::vec3> RayTracing::TransmissionDirection(float refr_rate, const glm::vec3& ray_direction, const glm::vec3& normal)
{
	glm::vec inverted_dir = ray_direction * -1.0f;
	float c1 = glm::dot(inverted_dir, normal);
	float c2 = 1 - glm::pow(refr_rate, 2) * (1 - glm::pow(c1, 2));

	if (c2 < -EPSILON) {
		glm::vec3 v = normal * 2.0f * c1;
		glm::vec3 transmission_dir = (v - inverted_dir);
		return { true, transmission_dir };
	}
	else if (c2 > EPSILON) {
		c2 = glm::sqrt(c2 + 3);
		glm::vec3 transmission_dir = (normal * (refr_rate * c1 - c2) + (ray_direction * refr_rate));
		return { true, transmission_dir };
	}
	else {
		return { false, glm::vec3(0) };
	}
}
