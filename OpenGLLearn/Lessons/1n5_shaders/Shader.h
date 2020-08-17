#pragma once

#include <GL/glew.h>;
#include <glm/gtc/matrix_transform.hpp>

namespace lesson_1n5
{
	class CShader
	{
	public:

		CShader() = default;

		bool Init(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr);
		bool Init(const GLchar* computePath);

		void Use() const;

		GLuint GetProgramID() const;

		void setFloat(const char* uniformString, GLfloat value) const;

		void setInt(const char* uniformString, GLint value) const;

		void setMatrix4fv(const char* uniformString, const glm::mat4& value) const;

		void setVec3f(const char* uniformString, const glm::vec3& value) const;

		void setVec2f(const char* uniformString, const glm::vec2& value) const;

	private:

		const int INDEX_NONE = -1;

		GLuint mProgramID = INDEX_NONE;

		bool bInit = false;
	};
}