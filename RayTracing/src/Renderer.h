#pragma once

#include "VertexArray.h"
#include "Camera.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>

class Renderer
{
public:
	void Init();

	void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	void SetDepthFunc(GLenum type);

	void BeginScene(Camera& camera);
	void EndScene();

	void SetClearColor(const glm::vec4& color);
	void Clear();

	void DrawIndexed(const std::shared_ptr<OpenGLVertexArray>& vertex_array);
	void CreateCubeMap(uint32_t* cubemap_texture, std::string* faces_cubemap);
private:
	struct SceneData {
		glm::mat4 view_projection_matrix;
	};


	static std::unique_ptr<SceneData> scene_data_;
};


