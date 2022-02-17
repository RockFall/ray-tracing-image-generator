#pragma once

#include <vector>
#include <memory>

#include <glad/glad.h>

#include "Buffer.h"

class OpenGLVertexArray
{
public:
	OpenGLVertexArray();
	~OpenGLVertexArray();

	// Bindings
	void Bind() const;
	void Unbind() const;

	// Add a vertex or index buffer to the vertex array
	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer);
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer);

	// Getters
	const std::vector<std::shared_ptr<VertexBuffer>>& get_vertex_buffers() const { return vertex_buffers_; };
	const std::shared_ptr<IndexBuffer>& get_index_buffer() const { return index_buffer_; };
private:
	uint32_t id_;

	std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers_;
	std::shared_ptr<IndexBuffer> index_buffer_;
};

