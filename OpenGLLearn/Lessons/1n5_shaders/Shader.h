#pragma once

#include <GL/glew.h>;

namespace lesson_1n5
{
	class CShader
	{
	public:

		CShader() = default;

		bool Init(const GLchar* vertexPath, const GLchar* fragmentPath);

		void Use();

		GLuint GetProgramID();

	private:

		GLuint mProgramID = -1;

		bool bInit = false;
	};
}