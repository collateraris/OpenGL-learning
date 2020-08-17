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
#include <random>

#include "../../Lessons/1n5_shaders/Shader.h"
#include "../../Lessons/1n9_camera/Camera.h"
#include "../../Lessons/3n1_assimp/LoadTexture.h"

namespace project_vol_clouds_opengl
{
	GLfloat g_screenWidth = 800.0f;
	GLfloat g_screenHeight = 600.0f;
	bool g_debug = false;
	int x_mouse_pos;
	int y_mouse_pos;

	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	float GetDeltaTime();

	void renderCube();

	void renderQuad();

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		lesson_1n5::CShader VolumeShader;
		if (!VolumeShader.Init("Projects/Real_time_volumetric_clouds/shaders/clouds.vs", "Projects/Real_time_volumetric_clouds/shaders/scene.fs")) return -1;

		lesson_1n5::CShader PerlinNoiseCompShader;
		if (!PerlinNoiseCompShader.Init("Projects/Real_time_volumetric_clouds/shaders/fBmPerlinNoise.cs")) return -1;

		lesson_1n5::CShader WorleyPerlinNoiseCompShader;
		if (!WorleyPerlinNoiseCompShader.Init("Projects/Real_time_volumetric_clouds/shaders/fBmWorleyNoise.cs")) return -1;

		lesson_1n5::CShader HighFreqNoiseCompShader;
		if (!HighFreqNoiseCompShader.Init("Projects/Real_time_volumetric_clouds/shaders/highFreqNoise.cs")) return -1;

		lesson_1n5::CShader LowFreqNoiseCompShader;
		if (!LowFreqNoiseCompShader.Init("Projects/Real_time_volumetric_clouds/shaders/lowFreqNoise.cs")) return -1;

		lesson_1n5::CShader WeatherMapCompShader;
		if (!WeatherMapCompShader.Init("Projects/Real_time_volumetric_clouds/shaders/weatherMapGen.cs")) return -1;

		lesson_1n5::CShader VolumeCloudsCompShader;
		if (!VolumeCloudsCompShader.Init("Projects/Real_time_volumetric_clouds/shaders/volumeClouds.cs")) return -1;

		unsigned int perlinNoiseTexW = 128, perlinNoiseTexH = 128, perlinNoiseTexD = 128;
		unsigned int perlinNoiseTexture = lesson_3n1::CLoadTexture::GetTexture3D(perlinNoiseTexW, perlinNoiseTexH, perlinNoiseTexD, GL_RGBA8, GL_RGBA, 
			GL_FLOAT, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

		PerlinNoiseCompShader.Use();
		PerlinNoiseCompShader.setInt("outTexture", 0);
		PerlinNoiseCompShader.setVec3f("uResolution", glm::vec3(perlinNoiseTexW, perlinNoiseTexH, perlinNoiseTexD));
		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, perlinNoiseTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glDispatchCompute(32, 32, 32);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		unsigned int worleyPerlinNoiseTexW = 128, worleyPerlinNoiseTexH = 128, worleyPerlinNoiseTexD = 128;
		unsigned int worleyPerlinNoiseTexture128 = lesson_3n1::CLoadTexture::GetTexture3D(worleyPerlinNoiseTexW, worleyPerlinNoiseTexH, worleyPerlinNoiseTexD, GL_RGBA8, GL_RGBA,
			GL_FLOAT, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

		WorleyPerlinNoiseCompShader.Use();
		WorleyPerlinNoiseCompShader.setInt("outTexture", 0);
		WorleyPerlinNoiseCompShader.setVec3f("uResolution", glm::vec3(worleyPerlinNoiseTexW, worleyPerlinNoiseTexH, worleyPerlinNoiseTexD));
		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, worleyPerlinNoiseTexture128, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glDispatchCompute(32, 32, 32);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		worleyPerlinNoiseTexW = worleyPerlinNoiseTexH = worleyPerlinNoiseTexD = 64;
		unsigned int worleyPerlinNoiseTexture64 = lesson_3n1::CLoadTexture::GetTexture3D(worleyPerlinNoiseTexW, worleyPerlinNoiseTexH, worleyPerlinNoiseTexD, GL_RGBA8, GL_RGBA,
			GL_FLOAT, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

		WorleyPerlinNoiseCompShader.Use();
		WorleyPerlinNoiseCompShader.setInt("outTexture", 0);
		WorleyPerlinNoiseCompShader.setVec3f("uResolution", glm::vec3(worleyPerlinNoiseTexW, worleyPerlinNoiseTexH, worleyPerlinNoiseTexD));
		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, worleyPerlinNoiseTexture64, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glDispatchCompute(16, 16, 16);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		worleyPerlinNoiseTexW = worleyPerlinNoiseTexH = worleyPerlinNoiseTexD = 32;
		unsigned int worleyPerlinNoiseTexture32 = lesson_3n1::CLoadTexture::GetTexture3D(worleyPerlinNoiseTexW, worleyPerlinNoiseTexH, worleyPerlinNoiseTexD, GL_RGBA8, GL_RGBA,
			GL_FLOAT, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

		WorleyPerlinNoiseCompShader.Use();
		WorleyPerlinNoiseCompShader.setInt("outTexture", 0);
		WorleyPerlinNoiseCompShader.setVec3f("uResolution", glm::vec3(worleyPerlinNoiseTexW, worleyPerlinNoiseTexH, worleyPerlinNoiseTexD));
		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, worleyPerlinNoiseTexture32, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glDispatchCompute(8, 8, 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		unsigned int highFreqNoiseTexW = 128, highFreqNoiseTexH = 128, highFreqNoiseTexD = 128;
		unsigned int highFreqNoiseTexture128 = lesson_3n1::CLoadTexture::GetTexture3D(highFreqNoiseTexW, highFreqNoiseTexH, highFreqNoiseTexD, GL_RGBA8, GL_RGBA,
			GL_FLOAT, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		HighFreqNoiseCompShader.Use();
		HighFreqNoiseCompShader.setInt("outTexture", 0);
		HighFreqNoiseCompShader.setInt("perlinNoise128", 1);
		HighFreqNoiseCompShader.setInt("worleyPerlinNoize128", 2);
		HighFreqNoiseCompShader.setInt("worleyPerlinNoize64", 3);
		HighFreqNoiseCompShader.setInt("worleyPerlinNoize32", 4);
		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, highFreqNoiseTexture128, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE1);
		glBindImageTexture(1, perlinNoiseTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE2);
		glBindImageTexture(2, worleyPerlinNoiseTexture128, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE3);
		glBindImageTexture(3, worleyPerlinNoiseTexture64, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE4);
		glBindImageTexture(4, worleyPerlinNoiseTexture32, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glDispatchCompute(32, 32, 32);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		unsigned int lowFreqNoiseTexW = 32, lowFreqNoiseTexH = 32, lowFreqNoiseTexD = 32;
		unsigned int lowFreqNoiseTexture32 = lesson_3n1::CLoadTexture::GetTexture3D(lowFreqNoiseTexW, lowFreqNoiseTexH, lowFreqNoiseTexD, GL_RGBA8, GL_RGBA,
			GL_FLOAT, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		LowFreqNoiseCompShader.Use();
		LowFreqNoiseCompShader.setInt("outTexture", 0);
		LowFreqNoiseCompShader.setInt("perlinNoise128", 1);
		LowFreqNoiseCompShader.setInt("worleyPerlinNoize128", 2);
		LowFreqNoiseCompShader.setInt("worleyPerlinNoize64", 3);
		LowFreqNoiseCompShader.setInt("worleyPerlinNoize32", 4);
		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, lowFreqNoiseTexture32, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE1);
		glBindImageTexture(1, perlinNoiseTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE2);
		glBindImageTexture(2, worleyPerlinNoiseTexture128, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE3);
		glBindImageTexture(3, worleyPerlinNoiseTexture64, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glActiveTexture(GL_TEXTURE4);
		glBindImageTexture(4, worleyPerlinNoiseTexture32, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
		glDispatchCompute(8, 8, 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		unsigned int weatherMapTexture = lesson_3n1::CLoadTexture::LoadGammaTexture("content/tex/weatherMap.png"); //lesson_3n1::CLoadTexture::GetTexture(g_screenWidth, g_screenHeight, GL_RGBA8, GL_RGBA);
		/*WeatherMapCompShader.Use();
		WeatherMapCompShader.setInt("outWeatherTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, weatherMapTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glDispatchCompute(g_screenWidth / 16, g_screenHeight / 16, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);*/

		unsigned int cloudsTexture = lesson_3n1::CLoadTexture::GetTexture(g_screenWidth, g_screenHeight, GL_RGBA32F, GL_RGBA);

		VolumeShader.Use();
		VolumeShader.setInt("uTexture", 0);

		VolumeCloudsCompShader.Use();
		VolumeCloudsCompShader.setVec3f("uParams.sunDir", glm::vec3(0, 0.1, -1));
		VolumeCloudsCompShader.setVec3f("uParams.sunColor", glm::vec3(1.));
		VolumeCloudsCompShader.setFloat("uProperties.density", 3.);
		VolumeCloudsCompShader.setFloat("uProperties.coverage", 1.);
		VolumeCloudsCompShader.setFloat("uProperties.attenuationT", 50.);
		VolumeCloudsCompShader.setFloat("uProperties.attenuationS", 15.);
		VolumeCloudsCompShader.setFloat("uProperties.sunIntensity", 150.);
		VolumeCloudsCompShader.setVec2f("uResolution", glm::vec2(g_screenWidth, g_screenHeight));


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		float deltaTime;

		while (!glfwWindowShouldClose(window))
		{
			deltaTime = GetDeltaTime();
			//std::cout << "FPS " << 1.f / deltaTime << std::endl;
			lesson_1n9::CCamera::Get().Movement(deltaTime);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			VolumeCloudsCompShader.Use();
			VolumeCloudsCompShader.setInt("outTexture", 0);
			VolumeCloudsCompShader.setInt("uLowFreqNoiseTex", 1);
			VolumeCloudsCompShader.setInt("uHighFreqNoiseTex", 2);
			VolumeCloudsCompShader.setInt("uWeatherMapTex", 3);
			VolumeCloudsCompShader.setVec3f("uViewDir", lesson_1n9::CCamera::Get().GetCameraFront());
			VolumeCloudsCompShader.setFloat("uParams.time", glfwGetTime());

			glActiveTexture(GL_TEXTURE0);
			glBindImageTexture(0, cloudsTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glActiveTexture(GL_TEXTURE1);
			glBindImageTexture(1, lowFreqNoiseTexture32, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
			glActiveTexture(GL_TEXTURE2);
			glBindImageTexture(2, highFreqNoiseTexture128, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
			glActiveTexture(GL_TEXTURE3);
			glBindImageTexture(3, weatherMapTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);
			glDispatchCompute(g_screenWidth / 4, g_screenHeight / 4, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			VolumeShader.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cloudsTexture);
			renderQuad();

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
			g_debug = !g_debug;
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		x_mouse_pos = xpos;
		y_mouse_pos = ypos;
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

	void renderCube()
	{
		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO = 0;

		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
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

	void renderQuad()
	{
		static unsigned int quadVAO = 0;
		static unsigned int quadVBO = 0;

		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}