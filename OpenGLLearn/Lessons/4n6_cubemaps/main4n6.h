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
#include "../3n1_assimp/AssimpData.h"
#include "../2n6_multy_lights/LightStates.h"

namespace lesson_4n6
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
		if (!sceneShader.Init("Lessons/4n6_cubemaps/shaders/scene.vs", "Lessons/4n6_cubemaps/shaders/scene.fs")) return -1;

		lesson_1n5::CShader transparentShader;
		if (!transparentShader.Init("Lessons/4n6_cubemaps/shaders/transparent.vs", "Lessons/4n6_cubemaps/shaders/transparent.fs")) return -1;

		lesson_1n5::CShader skyboxShader;
		if (!skyboxShader.Init("Lessons/4n6_cubemaps/shaders/skybox.vs", "Lessons/4n6_cubemaps/shaders/skybox.fs")) return -1;
		
		lesson_1n5::CShader obiwanShader;
		if (!obiwanShader.Init("Lessons/4n6_cubemaps/shaders/refraction.vs", "Lessons/4n6_cubemaps/shaders/refraction.fs")) return -1;

		lesson_3n1::SFileMeshData obiwan;
		lesson_3n1::CLoadAssimpFile::Load("content/model/obiwan/0.obj", obiwan);
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

		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
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

		//skybox
		unsigned int skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		unsigned int cubeTexture = lesson_3n1::CLoadTexture::loadTexture("content/tex/container.jpg");
		unsigned int floorTexture = lesson_3n1::CLoadTexture::loadTexture("content/tex/metal.png");
		unsigned int transparentTexture = lesson_3n1::CLoadTexture::loadTexture("content/tex/blending_transparent_window.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

		std::vector<std::string> faces = 
		{
			"content/cubemaps/FortPoint/right.jpg",
			"content/cubemaps/FortPoint/left.jpg",
			"content/cubemaps/FortPoint/top.jpg",
			"content/cubemaps/FortPoint/bottom.jpg",
			"content/cubemaps/FortPoint/front.jpg",
			"content/cubemaps/FortPoint/back.jpg"
		};
		unsigned int cubemapTexture = lesson_3n1::CLoadTexture::LoadCubemap(faces);

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

		skyboxShader.Use();
		skyboxShader.setMatrix4fv("projection", projection);
		transparentShader.setInt("screenTexture", 0);

		obiwanShader.Use();
		obiwanShader.setMatrix4fv("projection", projection);
		transparentShader.setInt("skybox", 0);

		lesson_3n1::CDrawFileMeshData::Init(obiwan);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// draw as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		while (!glfwWindowShouldClose(window))
		{
			lesson_1n9::CCamera::Get().Movement(GetDeltaTime());

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

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			obiwanShader.Use();

			obiwanShader.setMatrix4fv("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f));
			obiwanShader.setMatrix4fv("model", model);
			obiwanShader.setVec3f("cameraPos", cameraPos);

			lesson_3n1::CDrawFileMeshData::Draw(obiwanShader, obiwan);

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

			glDepthFunc(GL_LEQUAL);
			skyboxShader.Use();
			view = glm::mat4(glm::mat3(lesson_1n9::CCamera::Get().GetView()));
			skyboxShader.setMatrix4fv("view", view);
			glBindVertexArray(skyboxVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &planeVAO);
		glDeleteVertexArrays(1, &transparentVAO);
		glDeleteVertexArrays(1, &skyboxVAO);
		glDeleteBuffers(1, &cubeVBO);
		glDeleteBuffers(1, &planeVBO);
		glDeleteBuffers(1, &transparentVBO);
		glDeleteBuffers(1, &skyboxVBO);

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