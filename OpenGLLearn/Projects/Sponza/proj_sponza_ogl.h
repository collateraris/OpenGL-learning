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

#include "../../Lessons/1n5_shaders/Shader.h"
#include "../../Lessons/1n9_camera/Camera.h"
#include "../../Lessons/3n1_assimp/LoadTexture.h"
#include "../../Lessons/3n1_assimp/AssimpData.h"
#include "../../Lessons/2n6_multy_lights/LightStates.h"
#include "../../System/Frustum.h"
#include "../../System/Box.h"

namespace proj_sponza_ogl
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

	void renderQuad();

	int lesson_main()
	{
		GLFWwindow* window;
		if ((window = init()) == nullptr) return -1;

		lesson_1n5::CShader lightingShader;
		if (!lightingShader.Init("Projects/Sponza/shaders/lighting.vs", "Projects/Sponza/shaders/lightingMeshObj.fs")) return -1;

		lesson_1n5::CShader hdrToCubeMapShader;
		if (!hdrToCubeMapShader.Init("Projects/Sponza/shaders/hdrToCubeMap.vs", "Projects/Sponza/shaders/hdrToCubeMap.fs")) return -1;

		lesson_1n5::CShader envCubeMapShader;
		if (!envCubeMapShader.Init("Projects/Sponza/shaders/envCubeMap.vs", "Projects/Sponza/shaders/envCubeMap.fs")) return -1;

		lesson_1n5::CShader irradianceMapShader;
		if (!irradianceMapShader.Init("Projects/Sponza/shaders/irradianceMap.vs", "Projects/Sponza/shaders/irradianceMap.fs")) return -1;

		lesson_1n5::CShader prefilterMapShader;
		if (!prefilterMapShader.Init("Projects/Sponza/shaders/prefilterMap.vs", "Projects/Sponza/shaders/prefilterMap.fs")) return -1;

		lesson_1n5::CShader integrateBRDFMapShader;
		if (!integrateBRDFMapShader.Init("Projects/Sponza/shaders/integrateBRDFMap.vs", "Projects/Sponza/shaders/integrateBRDFMap.fs")) return -1;
		
		lesson_3n1::SFileMeshData meshObj;
		lesson_3n1::CLoadAssimpFile::Load("content/model/sponza/sponza.obj", meshObj);

		GLfloat aspectRatio = g_screenWidth / g_screenHeight;

		float nearDist = 0.1f;
		float farDist = 100.f;
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, nearDist, farDist);

		System::CFrustum frustum;

		lightingShader.Use();
		lightingShader.setInt("uIrradianceMap", 4);
		lightingShader.setInt("uPrefilterMap", 5);
		lightingShader.setInt("uBrdfLUT", 6);

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

		std::vector<glm::mat4> captureProjViews = {};
		for (const auto& view: captureViews)
		{
			captureProjViews.push_back(captureProjection * view);
		}

		unsigned int hdrTexture = lesson_3n1::CLoadTexture::LoadHDRTexture("content/tex/hdr/newport_loft.hdr");

		unsigned int envCubemap = lesson_3n1::CLoadTexture::GetEnvironmentCubemap(captureWidth, captureHeight);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		hdrToCubeMapShader.Use();
		hdrToCubeMapShader.setInt("uEquirectangularMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glViewport(0, 0, captureWidth, captureHeight);
		for (unsigned int i = 0; i < captureViews.size(); ++i)
		{
			hdrToCubeMapShader.setMatrix4fv("uProjectionView", captureProjViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		envCubeMapShader.Use();
		envCubeMapShader.setInt("uEnvironmentMap", 0);

		unsigned int irradianceMapWidth = 32, irradianceMapHeight = 32;
		unsigned int irradianceMap = lesson_3n1::CLoadTexture::GetEnvironmentCubemap(irradianceMapWidth, irradianceMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceMapWidth, irradianceMapHeight);
		irradianceMapShader.Use();
		irradianceMapShader.setInt("uEnvironmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glViewport(0, 0, irradianceMapWidth, irradianceMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < captureViews.size(); ++i)
		{
			irradianceMapShader.setMatrix4fv("uProjectionView", captureProjViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		unsigned int prefilterMapWidth = 128, prefilterMapHeight = 128;
		unsigned int prefilterMap = lesson_3n1::CLoadTexture::GetEnvironmentMipmapCubemap(prefilterMapWidth, prefilterMapHeight);
		prefilterMapShader.Use();
		prefilterMapShader.setInt("uEnvironmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = prefilterMapWidth * std::pow(0.5, mip);
			unsigned int mipHeight = prefilterMapHeight * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = static_cast<float>(mip) / (maxMipLevels - 1);
			prefilterMapShader.setFloat("uRoughness", roughness);
			for (unsigned int i = 0; i < captureViews.size(); ++i)
			{
				prefilterMapShader.setMatrix4fv("uProjectionView", captureProjViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				renderCube();
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		unsigned int brdfLUTWidth = 512, brdfLUTHeight = 512;
		unsigned int brdfLUTTexture = lesson_3n1::CLoadTexture::GetTexture(brdfLUTWidth, brdfLUTHeight, GL_RG16F, GL_RG,
									GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, brdfLUTWidth, brdfLUTHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
		glViewport(0, 0, brdfLUTWidth, brdfLUTHeight);
		integrateBRDFMapShader.Use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		float deltaTime;
		lesson_3n1::CDrawFileMeshData::Init(meshObj);
		glViewport(0, 0, g_screenWidth, g_screenHeight);
		while (!glfwWindowShouldClose(window))
		{
			deltaTime = GetDeltaTime();
			std::cout << "FPS " << 1.f / deltaTime << std::endl;
			lesson_1n9::CCamera::Get().Movement(deltaTime);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			lightingShader.Use();
			const glm::mat4& view = lesson_1n9::CCamera::Get().GetView();
			glm::mat4 proj_view = projection * view;
			lightingShader.setMatrix4fv("uProjectionView", proj_view);
			lightingShader.setVec3f("uViewPos", lesson_1n9::CCamera::Get().GetCameraPosition());
			glm::mat4 model;

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
				lightingShader.setMatrix4fv("uModel", model);
				renderSphere();
			}
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.));
			lightingShader.setMatrix4fv("uModel", model);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			frustum.calculateFrustum(lesson_1n9::CCamera::Get().GetView(), projection);
			for (const lesson_3n1::SMesh& mesh : meshObj.meshes)
			{
				if (frustum.boxInFrustum(mesh.GetMinBB(), mesh.GetMaxBB()))
				{
					lesson_3n1::CDrawFileMeshData::MeshDraw(lightingShader, mesh);
				}
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

		lesson_3n1::CDrawFileMeshData::DeleteAfterLoop(meshObj);
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

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
					double xSegment = static_cast<double>(x) / X_SEGMENTS;
					double ySegment = static_cast<double>(y) / Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0 * M_PI) * std::sin(ySegment * M_PI);
					float yPos = std::cos(ySegment * M_PI);
					float zPos = std::sin(xSegment * 2.0 * M_PI) * std::sin(ySegment * M_PI);

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