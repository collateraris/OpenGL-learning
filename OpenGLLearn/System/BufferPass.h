#pragma once

#include "../../Lessons/1n5_shaders/Shader.h"

namespace System
{
	class BufferPass
	{
	public:
		bool LoadShader(const char* vertexPath, const char* fragmentPath);

		lesson_1n5::CShader& GetShader();

		void InitBuffer(const float& screenW, const float& screenH) {};

		void StartDrawInBuffer() {};
		void EndDrawInBuffer() {};

	protected:
		lesson_1n5::CShader m_BufferShader;
	};
}