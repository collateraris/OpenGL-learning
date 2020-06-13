#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <iostream>

namespace lesson_1n3
{
	int main1n3();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	int main1n3()
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
			return -1;
		}
		glfwMakeContextCurrent(window);

		//keycallback
		glfwSetKeyCallback(window, key_callback);

		//инициализировать GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			return -1;
		}

		//Viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);

		//игровой цикл
		while (!glfwWindowShouldClose(window))
		{
			// Проверяем события и вызываем функции обратного вызова.
			glfwPollEvents();

			// Команды отрисовки здесь
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Меняем буферы местами
			glfwSwapBuffers(window);
		}

		glfwTerminate();
		return 0;		
	}


	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		// Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true, 
		// и приложение после этого закроется
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
}