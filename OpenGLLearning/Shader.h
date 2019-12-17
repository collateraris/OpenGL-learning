#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL/glew.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = "")
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;

		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::badbit);

		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			gShaderFile.open(geometryPath);
			std::stringstream vShaderStream, fShaderStream, gShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			gShaderStream << gShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();
			gShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			geometryCode = gShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		const GLchar* gShaderCode = geometryCode.c_str();

		// 2. —борка шейдеров
		GLuint vertex, fragment, geometry;
		GLint success;
		GLchar infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		if (geometryPath != "")
		{
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, nullptr);
			glCompileShader(geometry);

			glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
			};
		}

		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		if (geometryPath != "")
		{
			glAttachShader(this->Program, geometry);
		}
		glLinkProgram(this->Program);

		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		// ”дал€ем шейдеры, поскольку они уже в программу и нам больше не нужны.
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != "")
		{
			glDeleteShader(geometry);
		}
	}
	void Use()
	{
		glUseProgram(this->Program);
	}

	void setVec3f(const char* InShaderVariable, GLfloat x, GLfloat y, GLfloat z)
	{
		GLint Loc = glGetUniformLocation(this->Program, InShaderVariable);
		glUniform3f(Loc, x, y, z);
	}

	void setVec3f(const char* InShaderVariable, glm::vec3 InCoordinates)
	{
		GLint Loc = glGetUniformLocation(this->Program, InShaderVariable);
		glUniform3f(Loc, InCoordinates.x, InCoordinates.y, InCoordinates.z);
	}


	void setFloat(const char* InShaderVariable, GLfloat x)
	{
		GLint Loc = glGetUniformLocation(this->Program, InShaderVariable);
		glUniform1f(Loc, x);
	}

	void setInt(const char* InShaderVariable, GLint x)
	{
		GLint Loc = glGetUniformLocation(this->Program, InShaderVariable);
		glUniform1f(Loc, x);
	}

	void setMat4(const char* InShaderVariable, glm::mat4 x)
	{
		GLint Loc = glGetUniformLocation(this->Program, InShaderVariable);
		glUniformMatrix4fv(Loc, 1, GL_FALSE, glm::value_ptr(x));
	}
};

#endif