#include "BufferPass.h"

using namespace System;

bool BufferPass::LoadShader(const char* vertexPath, const char* fragmentPath)
{
	return m_BufferShader.Init(vertexPath, fragmentPath);
}

lesson_1n5::CShader& BufferPass::GetShader()
{
	return m_BufferShader;
}