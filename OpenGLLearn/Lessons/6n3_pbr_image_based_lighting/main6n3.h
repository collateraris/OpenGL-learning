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
#define _USE_MATH_DEFINES
#include <math.h>

#include "../1n5_shaders/Shader.h"
#include "../1n9_camera/Camera.h"
#include "../3n1_assimp/LoadTexture.h"
#include "../3n1_assimp/AssimpData.h"
#include "../2n6_multy_lights/LightStates.h"

namespace lesson_6n3
{
	GLfloat g_screenWidth = 800.0f;
	GLfloat g_screenHeight = 600.0f;
	bool g_blinn = false;

	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	float GetDeltaTime();

	void renderSphere();

	void renderCube();

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		lesson_1n5::CShader lightingShader;
		if (!lightingShader.Init("Lessons/6n3_pbr_image_based_lighting/shaders/lighting.vs", "Lessons/6n3_pbr_image_based_lighting/shaders/lighting.fs")) return -1;

		lesson_1n5::CShader hdrToCubeMapShader;
		if (!hdrToCubeMapShader.Init("Lessons/6n3_pbr_image_based_lighting/shaders/hdrToCubeMap.vs", "Lessons/6n3_pbr_image_based_lighting/shaders/hdrToCubeMap.fs")) return -1;

		lesson_1n5::CShader envCubeMapShader;
		if (!envCubeMapShader.Init("Lessons/6n3_pbr_image_based_lighting/shaders/envCubeMap.vs", "Lessons/6n3_pbr_image_based_lighting/shaders/envCubeMap.fs")) return -1;

		GLfloat aspectRatio = g_screenWidth / g_screenHeight;

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, 0.1f, 100.0f);

		lightingShader.Use();
		lightingShader.setInt("uAlbedoMap", 0);
		lightingShader.setInt("uMetallicMap", 1);
		lightingShader.setInt("uRoughnessMap", 2);
		lightingShader.setInt("uAoMap", 3);
		lightingShader.setInt("uNormalMap", 4);

		const int PBR_TEXTURE_NUMBER = 5;
		unsigned int albedoTexture[PBR_TEXTURE_NUMBER];
		unsigned int normalTexture[PBR_TEXTURE_NUMBER];
		unsigned int metallicTexture[PBR_TEXTURE_NUMBER];
		unsigned int roughnessTexture[PBR_TEXTURE_NUMBER];
		unsigned int aoTexture[PBR_TEXTURE_NUMBER];

		albedoTexture[0] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/RustedIron/albedo.png");
		normalTexture[0] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/RustedIron/normal.png");
		metallicTexture[0] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/RustedIron/metallic.png");
		roughnessTexture[0] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/RustedIron/roughness.png");
		aoTexture[0] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/RustedIron/ao.png");

		albedoTexture[1] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Gold/albedo.png");
		normalTexture[1] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Gold/normal.png");
		metallicTexture[1] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Gold/metallic.png");
		roughnessTexture[1] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Gold/roughness.png");
		aoTexture[1] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Gold/ao.png");

		albedoTexture[2] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Grass/albedo.png");
		normalTexture[2] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Grass/normal.png");
		metallicTexture[2] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Grass/metallic.png");
		roughnessTexture[2] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Grass/roughness.png");
		aoTexture[2] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Grass/ao.png");

		albedoTexture[3] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Plastic/albedo.png");
		normalTexture[3] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Plastic/normal.png");
		metallicTexture[3] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Plastic/metallic.png");
		roughnessTexture[3] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Plastic/roughness.png");
		aoTexture[3] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Plastic/ao.png");

		albedoTexture[4] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Wall/albedo.png");
		normalTexture[4] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Wall/normal.png");
		metallicTexture[4] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Wall/metallic.png");
		roughnessTexture[4] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Wall/roughness.png");
		aoTexture[4] = lesson_3n1::CLoadTexture::LoadTexture("content/tex/pbr/Wall/ao.png");

		const float spacing = 2.5;

		std::vector<glm::vec3> lightPositions = {
			glm::vec3(-10.0f,  10.0f, 10.0f),
			glm::vec3( 10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3( 10.0f, -10.0f, 10.0f),
		};

		std::vector<glm::vec3> lightColors = {
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f)
		};

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		unsigned int captureWidth = 512, captureHeight = 512;
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, captureWidth, captureHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		unsigned int envCubemap = lesson_3n1::CLoadTexture::GetEnvironmentCubemap(captureWidth, captureHeight);

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		std::vector<glm::mat4> captureViews =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		unsigned int hdrTexture = lesson_3n1::CLoadTexture::LoadHDRTexture("content/tex/hdr/Circus_Backstage_3k.hdr");

		hdrToCubeMapShader.Use();
		hdrToCubeMapShader.setInt("uEquirectangularMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glViewport(0, 0, captureWidth, captureHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < captureViews.size(); ++i)
		{
			glm::mat4 captureProjectionView = captureProjection * captureViews[i];
			hdrToCubeMapShader.setMatrix4fv("uProjectionView", captureProjectionView);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube(); 
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		envCubeMapShader.Use();
		envCubeMapShader.setInt("uEnvironmentMap", 0);

		float deltaTime;

		glViewport(0, 0, g_screenWidth, g_screenHeight);
		while (!glfwWindowShouldClose(window))
		{
			deltaTime = GetDeltaTime();
			//std::cout << "FPS " << 1.f / deltaTime << std::endl;
			lesson_1n9::CCamera::Get().Movement(deltaTime);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			lightingShader.Use();
			const glm::mat4& view = lesson_1n9::CCamera::Get().GetView();
			glm::mat4 proj_view = projection * view;
			lightingShader.setMatrix4fv("uProjectionView", proj_view);
			lightingShader.setVec3f("uViewPos", lesson_1n9::CCamera::Get().GetCameraPosition());


			glm::mat4 model = glm::mat4(1.0f);
			for (int row = 0; row < PBR_TEXTURE_NUMBER; ++row)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, albedoTexture[row]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, metallicTexture[row]);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, roughnessTexture[row]);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, aoTexture[row]);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, normalTexture[row]);

				for (int col = 0; col < PBR_TEXTURE_NUMBER; ++col)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(col - (PBR_TEXTURE_NUMBER >> 1)) * spacing,
						(row - (PBR_TEXTURE_NUMBER >> 1)) * spacing,
						0.0f
					));
					lightingShader.setMatrix4fv("uModel", model);
					renderSphere();
				}
			}

			for (unsigned int i = 0; i < lightPositions.size(); ++i)
			{
				glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				newPos = lightPositions[i];
				std::string uniformString = "lightPositions[" + std::to_string(i) + "]";
				lightingShader.setVec3f(uniformString.c_str(), newPos);
				uniformString = "lightColors[" + std::to_string(i) + "]";
				lightingShader.setVec3f(uniformString.c_str(), lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, newPos);
				model = glm::scale(model, glm::vec3(0.5f));
				lightingShader.setMatrix4fv("model", model);
				renderSphere();
			}

			envCubeMapShader.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
			glm::mat4 proj_rot_view = projection * glm::mat4(glm::mat3(view));
			envCubeMapShader.setMatrix4fv("uProjectionRotView", proj_rot_view);
			renderCube();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwTerminate();
		return 0;		
	}

	GLFWwindow* init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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

		if (key == GLFW_KEY_B && action == GLFW_PRESS)
			g_blinn = !g_blinn;
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

	void renderSphere()
	{
		static unsigned int sphereVAO = 0;
		static unsigned int indexCount;

		if (sphereVAO == 0)
		{
			glGenVertexArrays(1, &sphereVAO);

			unsigned int vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			std::vector<glm::vec3> positions = {};
			std::vector<glm::vec2> uv = {};
			std::vector<glm::vec3> normals = {};
			std::vector<unsigned int> indices = {};

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = static_cast<float>(x) / X_SEGMENTS;
					float ySegment = static_cast<float>(y) / Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
					float yPos = std::cos(ySegment * M_PI);
					float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

					positions.emplace_back(glm::vec3(xPos, yPos, zPos));
					uv.emplace_back(glm::vec2(xSegment, ySegment));
					normals.emplace_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}
			indexCount = indices.size();

			std::vector<float> data = {};
			for (unsigned int i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);
				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
			}

			glBindVertexArray(sphereVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
			float stride = (3 + 3 + 2) * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	}

	void renderCube()
	{
		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO = 0;

		if (cubeVAO == 0)
		{
			float vertices[] = {
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				// bottom face
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left         
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}