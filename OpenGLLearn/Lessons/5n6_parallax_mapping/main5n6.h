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

namespace lesson_5n6
{
	GLfloat g_screenWidth = 800.0f;
	GLfloat g_screenHeight = 600.0f;
	bool g_debug = false;

	int lesson_main();

	GLFWwindow* init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	float GetDeltaTime();

	void renderCube();

	void renderScene(const lesson_1n5::CShader& shader);

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		lesson_1n5::CShader lightingShader;
		if (!lightingShader.Init("Lessons/5n6_parallax_mapping/shaders/lighting.vs", "Lessons/5n6_parallax_mapping/shaders/lighting.fs")) return -1;

		// load textures
		// -------------
		unsigned int diffuseTex = lesson_3n1::CLoadTexture::LoadGammaTexture("content/tex/toy_box_diffuse.png");
		unsigned int normalTex = lesson_3n1::CLoadTexture::LoadNormalTexture("content/tex/toy_box_normal.png");
		unsigned int displacementTex = lesson_3n1::CLoadTexture::LoadTexture("content/tex/toy_box_disp.png");

		GLfloat aspectRatio = g_screenWidth / g_screenHeight;

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, 0.1f, 100.0f);

		lightingShader.Use();
		lightingShader.setMatrix4fv("projection", projection);
		lightingShader.setInt("diffuseTexture", 0);
		lightingShader.setInt("normalTexture", 1);
		lightingShader.setInt("displacementTexture", 2);

		float deltaTime;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glm::vec3 lightPos(0.5f, 1.0f, 0.3f);
		float heightScale = 0.1;

		while (!glfwWindowShouldClose(window))
		{
			deltaTime = GetDeltaTime();
			//std::cout << "FPS " << 1.f / deltaTime << std::endl;
			lesson_1n9::CCamera::Get().Movement(deltaTime);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glViewport(0, 0, g_screenWidth, g_screenHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lightingShader.Use();
			lightingShader.setVec3f("viewPos", lesson_1n9::CCamera::Get().GetCameraPosition());
			lightingShader.setVec3f("lightPos", lightPos);
			glm::mat4 view = lesson_1n9::CCamera::Get().GetView();
			lightingShader.setMatrix4fv("view", view);
			lightingShader.setFloat("height_scale", heightScale);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTex);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalTex);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, displacementTex);
			renderScene(lightingShader);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

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

		if (key == GLFW_KEY_B && action == GLFW_PRESS)
			g_debug = !g_debug;
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

	void renderCube()
	{
		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO = 0;

		if (cubeVAO == 0)
		{
			auto tangent_calc = [&](const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3, const glm::vec3& pos4,
				const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, const glm::vec2& uv4, glm::vec3& tangent1, glm::vec3& tangent2) noexcept
			{
				{
					glm::vec3 edge1 = pos2 - pos1;
					glm::vec3 edge2 = pos3 - pos1;
					glm::vec2 deltaUV1 = uv2 - uv1;
					glm::vec2 deltaUV2 = uv3 - uv1;

					float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

					tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
					tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
					tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
					tangent1 = glm::normalize(tangent1);
				}

				{
					glm::vec3 edge1 = pos4 - pos1;
					glm::vec3 edge2 = pos3 - pos1;
					glm::vec2 deltaUV1 = uv4 - uv1;
					glm::vec2 deltaUV2 = uv3 - uv1;

					float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

					tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
					tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
					tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
					tangent2 = glm::normalize(tangent2);
				}

			};

			// for back face
			// positions
			glm::vec3 pblbf(-1.0f, -1.0f, -1.0f); // bottom-left
			glm::vec3 pbrbf(1.0f, -1.0f, -1.0f); // bottom-right 
			glm::vec3 ptrbf(1.0f, 1.0f, -1.0f); // top-right
			glm::vec3 ptlbf(-1.0f, 1.0f, -1.0f); // top-left
			// texture coordinates
			glm::vec2 uvblbf(0.0f, 0.0f);
			glm::vec2 uvbrbf(1.0f, 0.0f);
			glm::vec2 uvtrbf(1.0f, 1.0f);
			glm::vec2 uvtlbf(0.0f, 1.0f);

			glm::vec3 tangent1bf;
			glm::vec3 tangent2bf;
			tangent_calc(pblbf, pbrbf, ptrbf, ptlbf, uvblbf, uvbrbf, uvtrbf, uvtlbf, tangent1bf, tangent2bf);

			// for front face
			// positions
			glm::vec3 pblff(-1.0f, -1.0f, 1.0f); // bottom-left
			glm::vec3 pbrff(1.0f, -1.0f, 1.0f); // bottom-right 
			glm::vec3 ptrff(1.0f, 1.0f, 1.0f); // top-right
			glm::vec3 ptlff(-1.0f, 1.0f, 1.0f); // top-left
			// texture coordinates
			glm::vec2 uvblff(0.0f, 0.0f);
			glm::vec2 uvbrff(1.0f, 0.0f);
			glm::vec2 uvtrff(1.0f, 1.0f);
			glm::vec2 uvtlff(0.0f, 1.0f);

			glm::vec3 tangent1ff;
			glm::vec3 tangent2ff;
			tangent_calc(pblff, pbrff, ptrff, ptlff, uvblff, uvbrff, uvtrff, uvtlff, tangent1ff, tangent2ff);

			// for left face
			// positions
			glm::vec3 pbllf(-1.0f, -1.0f, -1.0f); // bottom-left
			glm::vec3 pbrlf(-1.0f, -1.0f, 1.0f); // bottom-right 
			glm::vec3 ptrlf(-1.0f, 1.0f, 1.0f); // top-right
			glm::vec3 ptllf(-1.0f, 1.0f, -1.0f); // top-left
			// texture coordinates
			glm::vec2 uvbllf(0.0f, 1.0f);
			glm::vec2 uvbrlf(0.0f, 0.0f);
			glm::vec2 uvtrlf(1.0f, 0.0f);
			glm::vec2 uvtllf(1.0f, 1.0f);

			glm::vec3 tangent1lf;
			glm::vec3 tangent2lf;
			tangent_calc(pbllf, pbrlf, ptrlf, ptllf, uvbllf, uvbrlf, uvtrlf, uvtllf, tangent1lf, tangent2lf);

			// for right face
			// positions
			glm::vec3 pblrf(1.0f, -1.0f, 1.0f); // bottom-left
			glm::vec3 pbrrf(1.0f, -1.0f, -1.0f); // bottom-right 
			glm::vec3 ptrrf(1.0f, 1.0f, -1.0f); // top-right
			glm::vec3 ptlrf(1.0f, 1.0f, 1.0f); // top-left
			// texture coordinates
			glm::vec2 uvblrf(0.0f, 0.0f);
			glm::vec2 uvbrrf(0.0f, 1.0f);
			glm::vec2 uvtrrf(1.0f, 1.0f);
			glm::vec2 uvtlrf(1.0f, 0.0f);

			glm::vec3 tangent1rf;
			glm::vec3 tangent2rf;
			tangent_calc(pblrf, pbrrf, ptrrf, ptlrf, uvblrf, uvbrrf, uvtrrf, uvtlrf, tangent1rf, tangent2rf);

			// for bottom face
			// positions
			glm::vec3 pblbtf(1.0f, -1.0f, 1.0f); // bottom-left
			glm::vec3 pbrbtf(-1.0f, -1.0f,  1.0f); // bottom-right 
			glm::vec3 ptrbtf(-1.0f, -1.0f, -1.0f); // top-right
			glm::vec3 ptlbtf(1.0f, -1.0f, -1.0f); // top-left
			// texture coordinates
			glm::vec2 uvblbtf(1.0f, 0.0f);
			glm::vec2 uvbrbtf(0.0f, 0.0f);
			glm::vec2 uvtrbtf(0.0f, 1.0f);
			glm::vec2 uvtlbtf(1.0f, 1.0f);

			glm::vec3 tangent1btf;
			glm::vec3 tangent2btf;
			tangent_calc(pblbtf, pbrbtf, ptrbtf, ptlbtf, uvblbtf, uvbrbtf, uvtrbtf, uvtlbtf, tangent1btf, tangent2btf);

			// for top face
			// positions
			glm::vec3 pbltf(-1.0f, 1.0f, 1.0f); // bottom-left
			glm::vec3 pbrtf(1.0f, 1.0f, 1.0f); // bottom-right 
			glm::vec3 ptrtf(1.0f, 1.0f, -1.0f); // top-right
			glm::vec3 ptltf(-1.0f, 1.0f, -1.0f); // top-left
			// texture coordinates
			glm::vec2 uvbltf(0.0f, 0.0f);
			glm::vec2 uvbrtf(1.0f, 0.0f);
			glm::vec2 uvtrtf(1.0f, 1.0f);
			glm::vec2 uvtltf(0.0f, 1.0f);

			glm::vec3 tangent1tf;
			glm::vec3 tangent2tf;
			tangent_calc(pbltf, pbrtf, ptrtf, ptltf, uvbltf, uvbrtf, uvtrtf, uvtltf, tangent1tf, tangent2tf);

			float vertices[] = {
				// back face
				pblbf.x, pblbf.y, pblbf.z,  0.0f,  0.0f, -1.0f, uvblbf.x,  uvblbf.y, tangent1bf.x,  tangent1bf.y, tangent1bf.z, // bottom-left
				ptrbf.x, ptrbf.y, ptrbf.z,  0.0f,  0.0f, -1.0f, uvtrbf.x,  uvtrbf.y, tangent1bf.x,  tangent1bf.y, tangent1bf.z, // top-right
				pbrbf.x, pbrbf.y, pbrbf.z,  0.0f,  0.0f, -1.0f, uvbrbf.x,  uvbrbf.y, tangent1bf.x,  tangent1bf.y, tangent1bf.z, // bottom-right         
				ptrbf.x, ptrbf.y, ptrbf.z,  0.0f,  0.0f, -1.0f, uvtrbf.x,  uvtrbf.y, tangent2bf.x,  tangent2bf.y, tangent2bf.z, // top-right
				pblbf.x, pblbf.y, pblbf.z,  0.0f,  0.0f, -1.0f, uvblbf.x,  uvblbf.y, tangent2bf.x,  tangent2bf.y, tangent2bf.z, // bottom-left
				ptlbf.x, ptlbf.y, ptlbf.z,  0.0f,  0.0f, -1.0f, uvtlbf.x,  uvtlbf.y, tangent2bf.x,  tangent2bf.y, tangent2bf.z, // top-left
				// front face
				pblff.x, pblff.y, pblff.z,  0.0f,  0.0f,  1.0f, uvblff.x,  uvblff.y, tangent1ff.x,  tangent1ff.y, tangent1ff.z, // bottom-left
				pbrff.x, pbrff.y, pbrff.z,  0.0f,  0.0f,  1.0f, uvbrff.x,  uvbrff.y, tangent1ff.x,  tangent1ff.y, tangent1ff.z, // bottom-right
				ptrff.x, ptrff.y, ptrff.z,  0.0f,  0.0f,  1.0f, uvtrff.x,  uvtrff.y, tangent1ff.x,  tangent1ff.y, tangent1ff.z, // top-right
				ptrff.x, ptrff.y, ptrff.z,  0.0f,  0.0f,  1.0f, uvtrff.x,  uvtrff.y, tangent2ff.x,  tangent2ff.y, tangent2ff.z, // top-right
				ptlff.x, ptlff.y, ptlff.z,  0.0f,  0.0f,  1.0f, uvtlff.x,  uvtlff.y, tangent2ff.x,  tangent2ff.y, tangent2ff.z, // top-left
				pblff.x, pblff.y, pblff.z,  0.0f,  0.0f,  1.0f, uvblff.x,  uvblff.y, tangent2ff.x,  tangent2ff.y, tangent2ff.z, // bottom-left
				// left face
				ptrlf.x, ptrlf.y, ptrlf.z,  -1.0f,  0.0f,  0.0f, uvtrlf.x,  uvtrlf.y, tangent1lf.x,  tangent1lf.y, tangent1lf.z, // top-right
				ptllf.x, ptllf.y, ptllf.z,  -1.0f,  0.0f,  0.0f, uvtllf.x,  uvtllf.y, tangent1lf.x,  tangent1lf.y, tangent1lf.z, // top-left
				pbllf.x, pbllf.y, pbllf.z,  -1.0f,  0.0f,  0.0f, uvbllf.x,  uvbllf.y, tangent1lf.x,  tangent1lf.y, tangent1lf.z, // bottom-left
				pbllf.x, pbllf.y, pbllf.z,  -1.0f,  0.0f,  0.0f, uvbllf.x,  uvbllf.y, tangent2lf.x,  tangent2lf.y, tangent2lf.z, // bottom-left
				pbrlf.x, pbrlf.y, pbrlf.z,  -1.0f,  0.0f,  0.0f, uvbrlf.x,  uvbrlf.y, tangent2lf.x,  tangent2lf.y, tangent2lf.z, // bottom-right
				ptrlf.x, ptrlf.y, ptrlf.z,  -1.0f,  0.0f,  0.0f, uvtrlf.x,  uvtrlf.y, tangent2lf.x,  tangent2lf.y, tangent2lf.z, // top-right
				// right face
				ptlrf.x, ptlrf.y, ptlrf.z,  1.0f,  0.0f,  0.0f, uvtlrf.x,  uvtlrf.y, tangent1rf.x,  tangent1rf.y, tangent1rf.z, // top-left
				pbrrf.x, pbrrf.y, pbrrf.z,  1.0f,  0.0f,  0.0f, uvbrrf.x,  uvbrrf.y, tangent1rf.x,  tangent1rf.y, tangent1rf.z, // bottom-right
				ptrrf.x, ptrrf.y, ptrrf.z,  1.0f,  0.0f,  0.0f, uvtrrf.x,  uvtrrf.y, tangent1rf.x,  tangent1rf.y, tangent1rf.z, // top-right         
				pbrrf.x, pbrrf.y, pbrrf.z,  1.0f,  0.0f,  0.0f, uvbrrf.x,  uvbrrf.y, tangent2rf.x,  tangent2rf.y, tangent2rf.z, // bottom-right
				ptlrf.x, ptlrf.y, ptlrf.z,  1.0f,  0.0f,  0.0f, uvtlrf.x,  uvtlrf.y, tangent2rf.x,  tangent2rf.y, tangent2rf.z, // top-left
				pblrf.x, pblrf.y, pblrf.z,  1.0f,  0.0f,  0.0f, uvblrf.x,  uvblrf.y, tangent2rf.x,  tangent2rf.y, tangent2rf.z, // bottom-left     
				// bottom face
				ptrbtf.x, ptrbtf.y, ptrbtf.z,  0.0f, -1.0f,  0.0f, uvtrbtf.x,  uvtrbtf.y, tangent1btf.x,  tangent1btf.y, tangent1btf.z, // top-right
				ptlbtf.x, ptlbtf.y, ptlbtf.z,  0.0f, -1.0f,  0.0f, uvtlbtf.x,  uvtlbtf.y, tangent1btf.x,  tangent1btf.y, tangent1btf.z, // top-left
				pblbtf.x, pblbtf.y, pblbtf.z,  0.0f, -1.0f,  0.0f, uvblbtf.x,  uvblbtf.y, tangent1btf.x,  tangent1btf.y, tangent1btf.z, // bottom-left
				pblbtf.x, pblbtf.y, pblbtf.z,  0.0f, -1.0f,  0.0f, uvblbtf.x,  uvblbtf.y, tangent2btf.x,  tangent2btf.y, tangent2btf.z, // bottom-left
				pbrbtf.x, pbrbtf.y, pbrbtf.z,  0.0f, -1.0f,  0.0f, uvbrbtf.x,  uvbrbtf.y, tangent2btf.x,  tangent2btf.y, tangent2btf.z, // bottom-right
				ptrbtf.x, ptrbtf.y, ptrbtf.z,  0.0f, -1.0f,  0.0f, uvtrbtf.x,  uvtrbtf.y, tangent2btf.x,  tangent2btf.y, tangent2btf.z, // top-right
				// top face
				ptltf.x, ptltf.y, ptltf.z,  0.0f,  1.0f,  0.0f, uvtltf.x,  uvtltf.y, tangent1tf.x,  tangent1tf.y, tangent1tf.z, // top-left
				pbrtf.x, pbrtf.y, pbrtf.z,  0.0f,  1.0f,  0.0f, uvbrtf.x,  uvbrtf.y, tangent1tf.x,  tangent1tf.y, tangent1tf.z, // bottom-right
				ptrtf.x, ptrtf.y, ptrtf.z,  0.0f,  1.0f,  0.0f, uvtrtf.x,  uvtrtf.y, tangent1tf.x,  tangent1tf.y, tangent1tf.z, // top-right     
				pbrtf.x, pbrtf.y, pbrtf.z,  0.0f,  1.0f,  0.0f, uvbrtf.x,  uvbrtf.y, tangent2tf.x,  tangent2tf.y, tangent2tf.z, // bottom-right
				ptltf.x, ptltf.y, ptltf.z,  0.0f,  1.0f,  0.0f, uvtltf.x,  uvtltf.y, tangent2tf.x,  tangent2tf.y, tangent2tf.z, // top-left
				pbltf.x, pbltf.y, pbltf.z,  0.0f,  1.0f,  0.0f, uvbltf.x,  uvbltf.y, tangent2tf.x,  tangent2tf.y, tangent2tf.z, // bottom-left           
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void renderScene(const lesson_1n5::CShader& shader)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader.setMatrix4fv("model", model);
		renderCube();
	}
}