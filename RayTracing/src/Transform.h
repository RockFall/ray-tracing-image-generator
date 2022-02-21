#pragma once

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Describes Position, Scale and Rotation of an Object in the World
class Transform {
public:
	Transform() {}
	Transform(glm::vec3 position, glm::vec3 size = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f))
		: world_position_(position), world_scale_(size), yaw_pitch_roll_(rotation) {

		quat_orientation_ = glm::quat(yaw_pitch_roll_);
	}

	~Transform() {}
		
	// Model matrix (translate, rotate and scale)
	const glm::mat4 get_transform_matrix() const;

	// Position
	const glm::vec3& get_position()	const { return world_position_; }
	void add_position(const glm::vec3& position) { world_position_ += position;}
	void set_position(const glm::vec3& position) { world_position_ = position;}

	// Rotation
	const glm::vec3& get_yaw_pitch_roll()	const { return yaw_pitch_roll_; }
	void set_yaw_pitch_roll(const glm::vec3& yaw_pitch_roll);
	// TODO: Roll not yet supported
	void add_yaw(const float yaw);
	void add_pitch(const float pitch);

	//void add_rotation(const glm::vec3& rotation) { yaw_pitch_roll_ += rotation; has_changed_ = true; }

	// Scale
	void set_scale(const glm::vec3& scale) { world_scale_ = scale;}

	// Turns the orientation to a given target position
	void look_at(const glm::vec3& target, const glm::vec3& up_vec = glm::vec3(0.0f, 0.1f, 0.0f));

	// Get directions (all Quaternion based)
	glm::quat get_orientation() const;
	glm::vec3 get_up_direction() const;
	glm::vec3 get_right_direction() const;
	glm::vec3 get_forward_direction() const;
private:
	// World data
	glm::vec3 world_position_ = glm::vec3(0.0f);
	glm::vec3 yaw_pitch_roll_ = glm::vec3(0.0f);
	// TODO: this one can be tricky (see lossy scale in unity)
	glm::vec3 world_scale_ = glm::vec3(1.0f);
	// Quaternion main orientation
	glm::quat quat_orientation_ = glm::quat();
};