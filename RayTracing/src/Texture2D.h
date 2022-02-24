#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Texture2D
{
public:
	Texture2D() {}
	// Creates a texture from a given .ppm image
	Texture2D(const std::string& path);
	~Texture2D();

	uint32_t get_width() const { return img_.w; }
	uint32_t get_height() const { return img_.h; }

	// Return the color of a pixel in [x, y] of the texture
	glm::vec3 get_pixel(int x, int y) const { 
		RGB rgb = img_.data[(y * img_.w) + x];
		return glm::vec3(float(rgb.r)/255, float(rgb.g) / 255, float(rgb.b) / 255);
	}

private:
	struct RGB
	{
		unsigned char r, g, b;
	};

	struct ImageRGB
	{
		int w, h;
		std::vector<RGB> data;
	};
private:
	void load_ppm(const std::string& name);
	void eat_comment(std::ifstream& f);
private:
	std::string path_;
	ImageRGB img_ = ImageRGB();
};

