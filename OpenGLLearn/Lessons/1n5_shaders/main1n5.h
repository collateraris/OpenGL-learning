#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <iostream>

namespace lesson_1n5
{
	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		//������ �������
		GLuint vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 position;\n"
			"layout (location = 1) in vec3 color;"
			"out vec3 ourColor;"
			"void main()\n"
			"{\n"
			"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
			"ourColor = color;\n"
			"}\0";
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);


		{
			GLint success;
			GLchar infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
				return -1;
			}
		}

		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		const GLchar* fragmentShaderSource = "#version 330 core\n"
			"in vec3 ourColor;\n"
			"out vec4 color;\n"
			"void main()\n"
			"{\n"
			"color = vec4(ourColor, 1.0f);\n"
			"}\n\0";
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		{
			GLint success;
			GLchar infoLog[512];
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
				return -1;
			}
		}

		GLuint shaderProgram;
		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		{
			GLint success;
			GLchar infoLog[512];
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			}
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLfloat vertices[] = {
			// �������         // �����
			 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // ������ ������ ����
			-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // ������ ����� ����
			 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // ������� ����
		};

		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);

		//wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//������� ����
		while (!glfwWindowShouldClose(window))
		{
			// ��������� ������� � �������� ������� ��������� ������.
			glfwPollEvents();

			// ������� ��������� �����
			GLfloat timeValue = glfwGetTime();
			GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
			GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

			glUseProgram(shaderProgram);
			glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);

			// ������ ������ �������
			glfwSwapBuffers(window);
		}

		glfwTerminate();
		return 0;		
	}

	GLFWwindow* init()
	{
		//������������� GLFW
		glfwInit();
		//��������� GLFW
		//�������� ����������� ��������� ������ OpenGL. 
		//�������� 
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		//��������
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		//��������� �������� ��� �������� ��������� ��������
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//���������� ����������� ��������� ������� ����
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		//������� ������ ����
		GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return nullptr;
		}
		glfwMakeContextCurrent(window);

		//keycallback
		glfwSetKeyCallback(window, key_callback);

		//���������������� GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			return nullptr;
		}

		//Viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);

		return window;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		// ����� ������������ �������� ESC, �� ������������� �������� WindowShouldClose � true, 
		// � ���������� ����� ����� ���������
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
}