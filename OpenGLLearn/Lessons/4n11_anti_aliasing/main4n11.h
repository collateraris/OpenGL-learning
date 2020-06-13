#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include "../1n5_shaders/Shader.h"
#include "../1n9_camera/Camera.h"
#include "../3n1_assimp/LoadTexture.h"
#include "../3n1_assimp/AssimpData.h"
#include "../2n6_multy_lights/LightStates.h"

namespace lesson_4n11
{
	GLfloat g_screenWidth = 800.0f;
	GLfloat g_screenHeight = 600.0f;

	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	float GetDeltaTime();

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		lesson_1n5::CShader meshShader;
		if (!meshShader.Init("Lessons/4n11_anti_aliasing/shaders/mesh.vs", "Lessons/4n11_anti_aliasing/shaders/mesh.fs")) return -1;

		lesson_1n5::CShader instanceMeshShader;
		if (!instanceMeshShader.Init("Lessons/4n11_anti_aliasing/shaders/instance_mesh.vs", "Lessons/4n11_anti_aliasing/shaders/instance_mesh.fs")) return -1;

		lesson_3n1::SFileMeshData planetMesh;
		lesson_3n1::CLoadAssimpFile::Load("content/model/mars/planet.obj", planetMesh);
		lesson_3n1::SFileMeshData rockMesh;
		lesson_3n1::CLoadAssimpFile::Load("content/model/rock/rock.obj", rockMesh);

		unsigned int uniformBlockIndexScene = glGetUniformBlockIndex(meshShader.GetProgramID(), "Matrices");
		glUniformBlockBinding(meshShader.GetProgramID(), uniformBlockIndexScene, 0);

		unsigned int uboMatrices;
		glGenBuffers(1, &uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

		GLfloat aspectRatio = g_screenWidth / g_screenHeight;

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, 0.1f, 100.0f);

		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		unsigned int amount = 10000;
		std::vector<glm::mat4> modelMatrices = {};
		modelMatrices.reserve(amount);
		srand(glfwGetTime());
		float radius = 35.0;
		float offset = 10.f;

		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			float angle = (float)i / amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

			float y = displacement * 0.4f;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			float scale = (rand() % 20) / 100.0f + 0.05;
			model = glm::scale(model, glm::vec3(scale));

			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			modelMatrices.emplace_back(model);
		}

		lesson_3n1::CDrawFileMeshData::Init(planetMesh);
		lesson_3n1::CDrawFileMeshData::Init(rockMesh);

		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		for (unsigned int i = 0; i < rockMesh.meshes.size(); i++)
		{
			unsigned int VAO = rockMesh.meshes[i].GetVAO();
			glBindVertexArray(VAO);

			//for model mat4
			GLsizei vec4Size = sizeof(glm::vec4);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}

		lesson_2n6::CLightStates lightingState;
		lightingState.Init(lesson_2n6::EState::DESERT);
		meshShader.Use();
		lightingState.SetShaderParams(meshShader);
		instanceMeshShader.Use();
		lightingState.SetShaderParams(instanceMeshShader);

		float deltaTime;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_MULTISAMPLE);

		while (!glfwWindowShouldClose(window))
		{
			deltaTime = GetDeltaTime();
			//std::cout << "FPS " << 1.f / deltaTime << std::endl;
			lesson_1n9::CCamera::Get().Movement(deltaTime);

			lightingState.SetClearColorParam();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lesson_1n9::CCamera::Get().GetView()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glDisable(GL_CULL_FACE);

			meshShader.Use();
			meshShader.setVec3f("viewPos", lesson_1n9::CCamera::Get().GetCameraPosition());
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
			meshShader.setMatrix4fv("model", model);
			lesson_3n1::CDrawFileMeshData::Draw(meshShader, planetMesh);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			instanceMeshShader.Use();
			for (unsigned int i = 0; i < rockMesh.meshes.size(); i++)
			{
				unsigned int VAO = rockMesh.meshes[i].GetVAO();
				glBindVertexArray(VAO);
				glDrawElementsInstanced(
					GL_TRIANGLES, rockMesh.meshes[i].GetIndices().size(), GL_UNSIGNED_INT, 0, amount
				);
			}

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		lesson_3n1::CDrawFileMeshData::DeleteAfterLoop(planetMesh);
		lesson_3n1::CDrawFileMeshData::DeleteAfterLoop(rockMesh);

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
		glfwWindowHint(GLFW_SAMPLES, 4);

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

		//framebuffer
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, g_screenWidth, g_screenHeight);
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