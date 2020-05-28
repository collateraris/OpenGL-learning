#pragma once

#include <GL/glew.h>;
#include <glm/gtc/matrix_transform.hpp>

namespace lesson_1n5
{
	class CShader
	{
	public:

		CShader() = default;

		bool Init(const GLchar* vertexPath, const GLchar* fragmentPath);

		void Use();

		GLuint GetProgramID();

		void setFloat(const char* uniformString, GLfloat value);

		void setInt(const char* uniformString, GLint value);

		void setMatrix4fv(const char* uniformString, glm::mat4 value);

		void setVec3f(const char* uniformString, glm::vec3 value);

	private:

		GLuint mProgramID = -1;

		bool bInit = false;
	};
}