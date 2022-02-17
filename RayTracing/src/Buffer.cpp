#include "Buffer.h"

#include <glad/glad.h>

//////////////////////////////////////////////////////////////////////
//////////////////////////// VERTEX BUFFER ///////////////////////////
VertexBuffer::VertexBuffer(void* vertices, uint32_t size) : id_(0)
	{
	// Creates one VBO object
	glGenBuffers(1, &id_);
	// Binds it for it's use
	glBindBuffer(GL_ARRAY_BUFFER, id_);
	// Inserts vertices data into VBO
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}
VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &id_);
}
void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, id_);
}
void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
//////////////////////////// INDEX BUFFER ////////////////////////////
IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count) 
	: id_(0), count_(count)
{
	// Gera 1 objeto de EBO
	glGenBuffers(1, &id_);
	// Bind para a utilização do EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
	// Inserção dos indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}
IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &id_);
}
void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
}
void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
/////////////////////////////////////////////////////////////////////////
