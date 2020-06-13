#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <algorithm>

#include "../1n5_shaders/Shader.h"
#include "../1n9_camera/Camera.h"
#include "../2n6_multy_lights/LightStates.h"
#include "AssimpData.h"

namespace lesson_3n1
{
	GLfloat g_screenWidth = 800.0f;
	GLfloat g_screenHeight = 600.0f;

	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	float GetDeltaTime();

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		lesson_1n5::CShader backpackShader;
		if (!backpackShader.Init("Lessons/3n1_assimp/shaders/backpack.vs", "Lessons/3n1_assimp/shaders/backpack.fs")) return -1;

		SFileMeshData backpack;
		CLoadAssimpFile::Load("content/model/guitarBackpack/backpack.obj", backpack);

		GLfloat aspectRatio = g_screenWidth / g_screenHeight;

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection;

		backpackShader.Use();
		projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, 0.1f, 100.0f);
		backpackShader.setMatrix4fv("projection", projection);

		lesson_2n6::CLightStates specialLighting;
		specialLighting.Init(lesson_2n6::EState::DESERT);
		specialLighting.SetShaderParams(backpackShader);

		CDrawFileMeshData::Init(backpack);

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();

			lesson_1n9::CCamera::Get().Movement(GetDeltaTime());

			specialLighting.SetClearColorParam();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);

			backpackShader.Use();
	
			backpackShader.setVec3f("viewPos", lesson_1n9::CCamera::Get().GetCameraPosition());
			view = lesson_1n9::CCamera::Get().GetView();
			backpackShader.setMatrix4fv("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			backpackShader.setMatrix4fv("model", model);

			CDrawFileMeshData::Draw(backpackShader, backpack);

			glfwSwapBuffers(window);
		}

		CDrawFileMeshData::DeleteAfterLoop(backpack);

		glfwTerminate();
		return 0;		
	}

	GLFWwindow* init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


		GLFWwindow* window = glfwCreateWindow(g_screenWidth, g_screenHeight, "LearnOpenGL", nullptr, nullptr);
		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return nullptr;
		}
		glfwMakeContextCurrent(window);

		//keycallback
		glfwSetKeyCallback(window, key_callback);

		//mousecallback
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);

		//scrollcallback
		glfwSetScrollCallback(window, scroll_callback);

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
		lesson_1n9::CCamera::Get().KeyProcessing(key, action);

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		lesson_1n9::CCamera::Get().MouseProcessing(xpos, ypos);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		lesson_1n9::CCamera::Get().ScrollProcessing(xoffset, yoffset);
	}

	float GetDeltaTime()
	{
		static float deltaTime = 0.0f;	// Time between current frame and last frame
		static float lastFrame = 0.0f; // Time of last frame

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		return deltaTime;
	}
}