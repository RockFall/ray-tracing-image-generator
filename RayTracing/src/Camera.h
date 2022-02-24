#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Transform.h"

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up_vec, float fov, float width, float height)
		: transform_(Transform(position)), target_(target), field_of_view_(fov)
	{
		LookAtTarget();
	}

public:
	const glm::vec3& get_target()	const { return target_; }
	void set_target(const glm::vec3& target) { target_ = target; }
	Transform& get_transform() { return transform_;  }

	// Turns the Camera to given target
	void LookAtTarget() { transform_.look_at(target_); }
private:
	// -- World Space variables --
	Transform transform_;
	glm::vec3 target_;

	// Viewport
	float width_;
	float height_;

	// Angle of visibility from center
	float field_of_view_;
};


