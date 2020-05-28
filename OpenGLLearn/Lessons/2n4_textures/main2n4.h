#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image\stb_image.h>

#include "../1n5_shaders/Shader.h"
#include "../1n9_camera/Camera.h"

namespace lesson_2n4
{
	GLfloat g_mixTexture = 0.2f;

	GLfloat g_screenWidth = 800.0f;
	GLfloat g_screenHeight = 600.0f;

	struct Material
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

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

		lesson_1n5::CShader lightingShader;
		lesson_1n5::CShader lampShader;
		if (!lightingShader.Init("Lessons/2n4_textures/shaders/main.vs", "Lessons/2n4_textures/shaders/main.fs")) return -1;
		if (!lampShader.Init("Lessons/2n4_textures/shaders/main.vs", "Lessons/2n4_textures/shaders/lamp.fs")) return -1;

		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
		};

		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);


		GLuint diffuseMap;
		{
			glGenTextures(1, &diffuseMap);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			int width, height, nrComponents;
			unsigned char* image = stbi_load("content/tex/container2.png", &width, &height, &nrComponents, 0);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(image);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		GLuint lightVAO;
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		//wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		GLuint triangleShaderProgram = lightingShader.GetProgramID();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		GLfloat aspectRatio = g_screenWidth / g_screenHeight;

		glm::mat4 projection;

		glm::vec3 coral(1.0f, 0.5f, 0.31f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
		glm::vec3 cameraPos;

		glm::vec3 diffuseColor;
		glm::vec3 ambientColor;


		Material emerald;
		emerald.ambient = glm::vec3(0.0215,	0.1745,	0.0215);
		emerald.diffuse = glm::vec3(0.07568, 0.61424, 0.07568);
		emerald.specular = glm::vec3(0.633, 0.727811, 0.633);
		emerald.shininess = 77;

		Material silver;
		silver.ambient = glm::vec3(0.19225, 0.19225, 0.19225);
		silver.diffuse = glm::vec3(0.50754,	0.50754, 0.50754);
		silver.specular = glm::vec3(0.508273, 0.508273,	0.508273);
		silver.shininess = 52;

		Material cyanPlastic;
		cyanPlastic.ambient = glm::vec3(0.0f, 0.1f, 0.06f);
		cyanPlastic.diffuse = glm::vec3(0.0f, 0.50980392f, 0.50980392f);
		cyanPlastic.specular = glm::vec3(0.50196078f, 0.50196078f, 0.50196078f);
		cyanPlastic.shininess = 32;

		Material turquoise;
		turquoise.ambient = glm::vec3(0.1, 0.18725, 0.1745);
		turquoise.diffuse = glm::vec3(0.396, 0.74151, 0.69102);
		turquoise.specular = glm::vec3(0.297254, 0.30829, 0.306678);
		turquoise.shininess = 128 * 0.1;

		Material chrome;
		chrome.ambient = glm::vec3(0.25, 0.25, 0.25);
		chrome.diffuse = glm::vec3(0.4, 0.4, 0.4);
		chrome.specular = glm::vec3(0.774597, 0.774597, 0.774597);
		chrome.shininess = 128 * 0.6;

		Material currMaterial = emerald;

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();

			lesson_1n9::CCamera::Get().Movement(GetDeltaTime());

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);

			GLfloat radius = 5.0f;
			GLfloat camX = sin(glfwGetTime()) * radius;
			GLfloat camZ = cos(glfwGetTime()) * radius;
			lightPos = glm::vec3(camX, lightPos.y, camZ);

			view = lesson_1n9::CCamera::Get().GetView();
			projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, 0.1f, 100.0f);

			lampShader.Use();

			lampShader.setMatrix4fv("view", view);
			lampShader.setMatrix4fv("projection", projection);

			glBindVertexArray(VAO);
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f));
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

			lampShader.setMatrix4fv("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			lightingShader.Use();

			lightingShader.setInt("material.diffuse", 0);
			lightingShader.setVec3f("material.specular", currMaterial.specular);
			lightingShader.setFloat("material.shininess", currMaterial.shininess);

			lightingShader.setVec3f("light.ambient", lightColor);
			lightingShader.setVec3f("light.diffuse", lightColor); 
			lightingShader.setVec3f("light.specular", lightColor);

			lightingShader.setVec3f("lightPos", lightPos);

			lightingShader.setMatrix4fv("view", view);
			lightingShader.setMatrix4fv("projection", projection);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);

			glBindVertexArray(VAO);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			lightingShader.setMatrix4fv("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			glfwSwapBuffers(window);
		}

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		glfwTerminate();
		return 0;		
	}

	GLFWwindow* init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


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

		if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
			g_mixTexture = std::clamp(g_mixTexture - 0.1f, 0.0f, 1.0f);

		if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
			g_mixTexture = std::clamp(g_mixTexture + 0.1f, 0.0f, 1.0f);
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