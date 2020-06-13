#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <iostream>

#include "Shader.h"

namespace lesson_1n5
{
	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		//Сборка шейдера
		CShader triangleShader;
		if (!triangleShader.Init("Lessons/1n5_shaders/shaders/triangle.vs", "Lessons/1n5_shaders/shaders/triangle.fs")) return -1;


		GLfloat vertices[] = {
			// Позиции         // Цвета
			 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // Нижний правый угол
			-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // Нижний левый угол
			 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // Верхний угол
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

		GLfloat rightShiftInt = 0.5f;

		GLuint triangleShaderProgram = triangleShader.GetProgramID();

		//игровой цикл
		while (!glfwWindowShouldClose(window))
		{
			// Проверяем события и вызываем функции обратного вызова.
			glfwPollEvents();

			// Команды отрисовки здесь
			triangleShader.Use();
			glUniform1f(glGetUniformLocation(triangleShaderProgram, "rightShiftX"), rightShiftInt);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);

			// Меняем буферы местами
			glfwSwapBuffers(window);
		}

		glfwTerminate();
		return 0;		
	}

	GLFWwindow* init()
	{
		//Инициализация GLFW
		glfwInit();
		//Настройка GLFW
		//Задается минимальная требуемая версия OpenGL. 
		//Мажорная 
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		//Минорная
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		//Установка профайла для которого создается контекст
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//Выключение возможности изменения размера окна
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		//создать объект окна
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

		//инициализировать GLEW
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
		// Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true, 
		// и приложение после этого закроется
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
}