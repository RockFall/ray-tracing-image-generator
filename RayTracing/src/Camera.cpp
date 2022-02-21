#include "Camera.h"

// TODO: maybe remove?
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>

std::shared_ptr<Camera> Camera::main_ = nullptr;


Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up_vec, float fov, float width, float height)
	: transform_(Transform(position)), target_(target), field_of_view_(fov), aspect_(width / height)
{
	LookAtTarget();
	UpdateViewProjectionMatrix();
}

void Camera::UpdateProjectionMatrixOnly()
{
	projection_matrix_ = glm::perspective(	
		glm::radians(field_of_view_), 
		(float)aspect_,
		near_clip_plane_,
		far_clip_plane_);
}

void Camera::UpdateViewMatrixOnly()
{
	glm::quat orientation = transform_.get_orientation();
	view_matrix_ = glm::translate(glm::mat4(1.0f), transform_.get_position()) * glm::toMat4(orientation);
	view_matrix_ = glm::inverse(view_matrix_);
}

const glm::mat4& Camera::get_viewprojection_matrix()
{
	UpdateViewProjectionMatrix();
	return view_projection_matrix_;
}

const glm::mat4& Camera::get_projection_matrix()
{
	UpdateViewProjectionMatrix();
	return projection_matrix_;
}

const glm::mat4& Camera::get_view_matrix()
{
	UpdateViewProjectionMatrix();
	return view_matrix_;
}

void Camera::UpdateViewProjectionMatrix()
{
	UpdateProjectionMatrixOnly();
	UpdateViewMatrixOnly();

	view_projection_matrix_ = projection_matrix_ * view_matrix_;
}
