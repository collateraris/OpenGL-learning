#include "Buffer.h"

#include <GLFW\glfw3.h>

using namespace System;

Buffer::Buffer(GLenum target)
	: m_Target{ target }
{
	glCreateBuffers(1, &m_BufferHandle);
}

Buffer::~Buffer()
{
    if (glfwGetCurrentContext() != nullptr)
    {
        glDeleteBuffers(1, &m_BufferHandle);
    }
}

void Buffer::BindBase(unsigned int binding) const
{
	glBindBufferBase(m_Target, binding, m_BufferHandle);
}

GLuint Buffer::GetHandle() const
{
    return m_BufferHandle;
}