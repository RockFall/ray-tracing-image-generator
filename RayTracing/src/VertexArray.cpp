#include "VertexArray.h"

#include <assert.h>

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
	switch (type)
	{
		case ShaderDataType::None:		return GL_NONE;
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Bool:		return GL_BOOL;
	}
	std::cout << "ShaderDataTypeToOpenGLBaseType - The given type {0} is unknown" << std::endl;
	assert(false);
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray() : id_(0) {
	glGenVertexArrays(1, &id_);
	glBindVertexArray(id_);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &id_);
}

void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(id_);
}
void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}
void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer)
{
	glBindVertexArray(id_);
	vertex_buffer->Bind();

	if (vertex_buffer->get_layout().get_elements().size()) {
		std::cout << "Adding a VertexBuffer to the VertexArray without a layout" << std::endl;
		assert(false);
	}
	
	uint32_t index = 0;
	const auto& layout = vertex_buffer->get_layout();
	for (const auto& element : layout)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(
			index,
			element.get_component_count(),
			ShaderDataTypeToOpenGLBaseType(element.type),
			element.normalized ? GL_TRUE : GL_FALSE,
			vertex_buffer->get_layout().get_stride(),
			(const void*)element.offset
				
		);
		++index;
	}

	vertex_buffers_.push_back(vertex_buffer);
}
void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer)
{
	glBindVertexArray(id_);
	index_buffer->Bind();

	index_buffer_ = index_buffer;
}
