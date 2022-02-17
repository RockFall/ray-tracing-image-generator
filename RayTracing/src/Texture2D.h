#pragma once

#include <cstdint>
#include <string>

#include <glad/glad.h>

class Texture2D
{
public:
	Texture2D(uint32_t width, uint32_t height);
	Texture2D(const std::string& path);
	~Texture2D();

	uint32_t get_width() const { return width_; }
	uint32_t get_height() const { return height_; }
	uint32_t get_renderer_id() const { return renderer_id_; }

	void set_data(void* data, uint32_t size);

	void Bind(uint32_t slot = 0) const;

	bool is_loaded() const { return is_loaded_; }

	bool operator==(const Texture2D& other) const
	{
		return renderer_id_ == ((Texture2D&)other).renderer_id_;
	}

private:
	std::string path_;
	bool is_loaded_ = false;
	uint32_t width_, height_;
	uint32_t renderer_id_;
	GLenum internal_format_, data_format_;
};

