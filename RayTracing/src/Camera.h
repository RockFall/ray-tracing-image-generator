#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Transform.h"

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up_vec, float fov, float width, float height);
public:
			// SETTERS AND GETTERS //
	const glm::vec3& get_target()	const { return target_; }
	void set_target(const glm::vec3& target) { target_ = target; }

	Transform& get_transform() { return transform_;  }

	const glm::mat4& get_viewprojection_matrix();
	const glm::mat4& get_projection_matrix();
	const glm::mat4& get_view_matrix();

	static const std::shared_ptr<Camera>& get_main() { return main_; }
	static void set_main(const std::shared_ptr<Camera>& main) { main_ = main; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
	void LookAtTarget() { transform_.look_at(target_); }

private:
	// Updates both the View and Projection Matrix
	void UpdateViewProjectionMatrix();
	// Updates the PROJECTION Matrix (and the ViewProjection)
	void UpdateProjectionMatrixOnly();
	// Updates the VIEW Matrix (and the ViewProjection)
	void UpdateViewMatrixOnly();
private:
	//  STATIC PROPERTIES
	static std::shared_ptr<Camera> main_;
private:
	// -- World Space variables --
	Transform transform_;
	glm::vec3 target_;

	// Viewport
	float width_;
	float height_;

	// Angle of visibility from center (for PERSPECTIVE projection ONLY)
	float field_of_view_;
	// Width / Height
	float aspect_;

	// Distance from camera to NEAR clip plane
	float near_clip_plane_ = 1.0f;
	// Distance from camera to FAR clip plane
	float far_clip_plane_ = 100.0f;

	// Transforms the view space into clip space
	glm::mat4 projection_matrix_ = glm::mat4(0);
	// Inversion of the transformation matrix of the camera
	glm::mat4 view_matrix_ = glm::mat4(0);
	// Used isntead of doing the multiplication multiple times
	glm::mat4 view_projection_matrix_ = glm::mat4(0);
};


