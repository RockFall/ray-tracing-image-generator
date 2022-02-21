#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Texture2D
{
public:
	Texture2D() {}
	Texture2D(const std::string& path);
	~Texture2D();

	uint32_t get_width() const { return width_; }
	uint32_t get_height() const { return height_; }
	bool is_loaded() const { return is_loaded_; }

private:
	std::string path_;
	bool is_loaded_ = false;
	uint32_t width_, height_;
	
	std::vector<int> data;
};

