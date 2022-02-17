#include "Texture2D.h"

#include "stb_image.h"

#include <fstream>
#include <iostream>
#include <assert.h>

Texture2D::Texture2D(uint32_t width, uint32_t height)
	: width_(width), height_(height)
{
	internal_format_ = GL_RGBA8;
	data_format_ = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
	glTextureStorage2D(renderer_id_, 1, internal_format_, width_, height_);

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::Texture2D(const std::string& path)
	: path_(path), data_format_(), internal_format_(), width_(), height_(), renderer_id_(0)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		//GRT_PROFILE_SCOPE("stbi_load - Texture2D::Texture2D(const std::string&)");
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	}

	if (data)
	{
		is_loaded_ = true;

		width_ = width;
		height_ = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		internal_format_ = internalFormat;
		data_format_ = dataFormat;

		assert(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
		glTextureStorage2D(renderer_id_, 1, internalFormat, width_, height_);

		glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	else {
		std::cout << "stb_image could not load image file! (path=" << path << ")" << std::endl;
	}
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &renderer_id_);
}

void Texture2D::set_data(void* data, uint32_t size)
{
	uint32_t bpp = data_format_ == GL_RGBA ? 4 : 3;
	assert(size == width_ * height_ * bpp, "Data must be entire texture!");
	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data);
}

void Texture2D::Bind(uint32_t slot) const
{
	glBindTextureUnit(slot, renderer_id_);
}