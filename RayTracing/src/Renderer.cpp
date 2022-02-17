#include "Renderer.h"

#include "stb_image.h"

#include <iostream>

std::unique_ptr<Renderer::SceneData> Renderer::scene_data_ = std::make_unique<Renderer::SceneData>();

void Renderer::Init()
{
	glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	glEnable(GL_DEPTH_TEST);
}

void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void Renderer::SetDepthFunc(GLenum type)
{
	glDepthFunc(type);
}

void Renderer::BeginScene(Camera& camera)
{
	scene_data_->view_projection_matrix = camera.get_viewprojection_matrix();
}

void Renderer::EndScene()
{

}

void Renderer::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawIndexed(const std::shared_ptr<OpenGLVertexArray>& vertex_array)
{
	vertex_array->Bind();
	glDrawElements(GL_TRIANGLES, vertex_array->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::CreateCubeMap(uint32_t* cubemap_texture, std::string* faces_cubemap)
{
	// Creates the cubemap texture object
	glGenTextures(1, cubemap_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemap_texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(faces_cubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (stbi_failure_reason())
			std::cout << stbi_failure_reason();
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << faces_cubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}
}
