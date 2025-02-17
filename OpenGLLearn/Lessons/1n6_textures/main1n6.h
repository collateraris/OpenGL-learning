#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image\stb_image.h>

#include <iostream>
#include <algorithm>

#include "../1n5_shaders/Shader.h"

namespace lesson_1n6
{
	GLfloat g_mixTexture = 0.2f;

	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		//������ �������
		lesson_1n5::CShader triangleShader;
		if (!triangleShader.Init("Lessons/1n6_textures/shaders/triangle.vs", "Lessons/1n6_textures/shaders/triangle.fs")) return -1;


		GLfloat vertices[] = {
			// �������          // �����             // ���������� ����������
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // ������� ������
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // ������ ������
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ������ �����
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // ������� �����
		};

		GLuint indices[] = {  // Note that we start from 0!
			0, 1, 3, // First Triangle
			1, 2, 3  // Second Triangle
		};

		GLuint VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// �������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// ����� 
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// ���������� ����������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
		
		GLuint texture , texture1;
		{
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			int width, height, nrComponents;
			unsigned char* image = stbi_load("content/tex/container.jpg", &width, &height, &nrComponents, 0);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(image);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		{
			glGenTextures(1, &texture1);
			glBindTexture(GL_TEXTURE_2D, texture1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


			int width, height, nrComponents;
			unsigned char* image = stbi_load("content/tex/awesomeface.png", &width, &height, &nrComponents, 0);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(image);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		//wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		GLuint triangleShaderProgram = triangleShader.GetProgramID();

		//������� ����
		while (!glfwWindowShouldClose(window))
		{
			// ��������� ������� � �������� ������� ��������� ������.
			glfwPollEvents();

			// ������� ��������� �����
			triangleShader.Use();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(triangleShaderProgram, "ourTexture"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(triangleShaderProgram, "ourTexture1"), 1);

			glUniform1f(glGetUniformLocation(triangleShaderProgram, "mixTexture"), g_mixTexture);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// ������ ������ �������
			glfwSwapBuffers(window);
		}

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);

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

		if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
			g_mixTexture = std::clamp(g_mixTexture - 0.1f, 0.0f, 1.0f);

		if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
			g_mixTexture = std::clamp(g_mixTexture + 0.1f, 0.0f, 1.0f);
	}
}