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

#include "../1n5_shaders/Shader.h"
#include "../1n9_camera/Camera.h"
#include "../3n1_assimp/LoadTexture.h"

namespace lesson_4n5
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

		lesson_1n5::CShader sceneShader;
		if (!sceneShader.Init("Lessons/4n5_framebuffer/shaders/scene.vs", "Lessons/4n5_framebuffer/shaders/scene.fs")) return -1;

		lesson_1n5::CShader transparentShader;
		if (!transparentShader.Init("Lessons/4n5_framebuffer/shaders/transparent.vs", "Lessons/4n5_framebuffer/shaders/transparent.fs")) return -1;

		lesson_1n5::CShader framebufferShader;
		if (!framebufferShader.Init("Lessons/4n5_framebuffer/shaders/framebuffer_kernel_effect.vs", "Lessons/4n5_framebuffer/shaders/framebuffer_kernel_effect.fs")) return -1;
		// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
		float cubeVertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left                
			// Front face
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right        
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left        
			// Left face
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left       
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
			// Right face
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right      
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
			// Bottom face          
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
			// Top face
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left  
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left              
		};


		float planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
			 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
		};

		float transparentVertices[] = {
			// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
			1.0f,  0.5f,  0.0f,  1.0f,  0.0f
		};

		std::vector<glm::vec3> transparentObjPos = {
			glm::vec3(-1.5f, 0.0f, -0.48f),
			glm::vec3(1.5f, 0.0f, 0.51f),
			glm::vec3(0.0f, 0.0f, 0.7f),
			glm::vec3(-0.3f, 0.0f, -2.3f),
			glm::vec3(0.5f, 0.0f, -0.6f)
		};

		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
		};
		// cube VAO
		unsigned int cubeVAO, cubeVBO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
		// plane VAO
		unsigned int planeVAO, planeVBO;
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		// transparent VAO
		unsigned int transparentVAO, transparentVBO;
		glGenVertexArrays(1, &transparentVAO);
		glGenBuffers(1, &transparentVBO);
		glBindVertexArray(transparentVAO);
		glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		// quad VAO
		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);

		unsigned int framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		unsigned int framebufferTexture = lesson_3n1::CLoadTexture::GetFBOTexture(g_screenWidth, g_screenHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, g_screenWidth, g_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			return -1;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		unsigned int cubeTexture = lesson_3n1::CLoadTexture::loadTexture("content/tex/container.jpg");
		unsigned int floorTexture = lesson_3n1::CLoadTexture::loadTexture("content/tex/metal.png");
		unsigned int transparentTexture = lesson_3n1::CLoadTexture::loadTexture("content/tex/blending_transparent_window.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

		GLfloat aspectRatio = g_screenWidth / g_screenHeight;

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection;

		sceneShader.Use();
		projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, 0.1f, 100.0f);
		sceneShader.setMatrix4fv("projection", projection);
		sceneShader.setInt("texture1", 0);

		transparentShader.Use();
		transparentShader.setMatrix4fv("projection", projection);
		transparentShader.setInt("texture1", 0);

		framebufferShader.Use();
		transparentShader.setInt("screenTexture", 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// draw as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		while (!glfwWindowShouldClose(window))
		{
			lesson_1n9::CCamera::Get().Movement(GetDeltaTime());

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CW);

			sceneShader.Use();	
			const glm::vec3& cameraPos = lesson_1n9::CCamera::Get().GetCameraPosition();
			sceneShader.setVec3f("viewPos", cameraPos);
			view = lesson_1n9::CCamera::Get().GetView();
			sceneShader.setMatrix4fv("view", view);

			// floor
			glBindVertexArray(planeVAO);
			glBindTexture(GL_TEXTURE_2D, floorTexture);
			sceneShader.setMatrix4fv("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			//cube
			glBindVertexArray(cubeVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cubeTexture);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
			sceneShader.setMatrix4fv("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
			sceneShader.setMatrix4fv("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			//transparent object

			glDisable(GL_CULL_FACE);

			transparentShader.Use();
			transparentShader.setMatrix4fv("view", view);
			glBindVertexArray(transparentVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, transparentTexture);

			std::sort(transparentObjPos.begin(), transparentObjPos.end(), [&](glm::vec3& a, glm::vec3& b) {

				float distanceA = glm::length(cameraPos - a);
				float distanceB = glm::length(cameraPos - b);

				return distanceA > distanceB;
			});

			for (unsigned int i = 0; i < transparentObjPos.size(); i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, transparentObjPos[i]);
				transparentShader.setMatrix4fv("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			glBindVertexArray(0);

			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			framebufferShader.Use();
			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, framebufferTexture);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &planeVAO);
		glDeleteVertexArrays(1, &transparentVAO);
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &cubeVBO);
		glDeleteBuffers(1, &planeVBO);
		glDeleteBuffers(1, &transparentVBO);
		glDeleteBuffers(1, &quadVBO);
		glDeleteFramebuffers(1, &framebuffer);

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