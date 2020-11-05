#include "proj_voxel_gi.h"

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
#include "../../System/SparseVoxelOctree.h"
#include "../../System/VoxelGrid.h"
#include "../../System/StringConst.h"
#include "../../System/DepthBufferPass.h"

namespace proj_voxel_gi
{
	GLfloat g_screenWidth = 1200.0f;
	GLfloat g_screenHeight = 800.0f;
	const unsigned int shadowWidth = 1024, shadowHeight = 1024;

	GLFWwindow* init();

	void APIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	float GetDeltaTime();

	void renderQuad();

	GLFWwindow* init();
}

using namespace proj_voxel_gi;

int SponzaScene::lesson_main()
{
	GLFWwindow* window;
	if ((window = init()) == nullptr) return -1;

	lesson_1n5::CShader traverseOctreeShader;
	if (!traverseOctreeShader.Init("Projects/VoxelGI/shaders/traceSparseVoxelOctree.vert", "Projects/VoxelGI/shaders/traceSparseVoxelOctree.frag")) return -1;

	lesson_1n5::CShader voxelGridShader;
	if (!voxelGridShader.Init("System/shaders/VoxelGrid/voxelGridFill.vert", "System/shaders/VoxelGrid/voxelGridFill.frag", 
		"System/shaders/VoxelGrid/voxelGridFill.geom")) return -1;

	lesson_1n5::CShader voxelGridVisShader;
	if (!voxelGridVisShader.Init("System/shaders/VoxelGrid/voxelGridVisible.vert", "System/shaders/VoxelGrid/voxelGridVisible.frag")) return -1;

	System::DepthBufferPass depthRenderPass;
	depthRenderPass.InitBuffer(g_screenWidth, g_screenHeight);

	System::VoxelGridInfo info;
	info.depthMapId = depthRenderPass.GetDepthMap();
	System::VoxelGrid voxelGrid(info);
	return -1;

	lesson_3n1::SFileMeshData sponzaScene;
	lesson_3n1::CLoadAssimpFile::Load("content/model/sponza/sponza.obj", sponzaScene);

	GLfloat aspectRatio = g_screenWidth / g_screenHeight;

	float nearDist = 0.1f;
	float farDist = 1000.f;
	glm::mat4 projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, nearDist, farDist);
	glm::mat4 invProj = glm::inverse(projection);

	glm::vec3 lightPos = glm::vec3(-18.09f, 21.17f, -10.85f);
	glm::vec3 lightDir = glm::vec3(0.67f, -0.73f, 0.13f);
	glm::vec3 lightColor = glm::vec3(800.0f, 800.0f, 800.0f);

	lesson_1n9::CCamera::Get().SetCameraPosition(lightPos);
	lesson_1n9::CCamera::Get().SetCameraFront(lightDir);

	float deltaTime;
	lesson_3n1::CDrawFileMeshData::Init(sponzaScene);
	System::SparseVoxelOctree octree;
	octree.Init(sponzaScene.meshes, 5);

	traverseOctreeShader.Use();
	traverseOctreeShader.setVec3f(System::uBminStr.c_str(), octree.GetBMin());
	traverseOctreeShader.setVec3f(System::uBmaxStr.c_str(), octree.GetBMax());
	traverseOctreeShader.setInt(System::uMaxLevelStr.c_str(), 10);
	traverseOctreeShader.setMatrix4fv(System::uInvProjectionMatrixStr.c_str(), invProj);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glViewport(0, 0, g_screenWidth, g_screenHeight);

	while (!glfwWindowShouldClose(window))
	{
		
		deltaTime = GetDeltaTime();
		std::cout << "FPS " << 1.f / deltaTime << std::endl;
		lesson_1n9::CCamera::Get().Movement(deltaTime);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const glm::mat4 model = glm::mat4(1.0f);
		const glm::mat4 view = lesson_1n9::CCamera::Get().GetView();
		const glm::mat4 invView = glm::inverse(view);
		const glm::vec3& camPos = lesson_1n9::CCamera::Get().GetCameraPosition();

		octree.Update();
		traverseOctreeShader.Use();
		octree.Bind();
		traverseOctreeShader.setMatrix4fv(System::uInvViewMatrixStr.c_str(), invView);
		traverseOctreeShader.setVec3f(System::uCamPosStr.c_str(), camPos);
		renderQuad();
	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	lesson_3n1::CDrawFileMeshData::DeleteAfterLoop(sponzaScene);
	glfwTerminate();
	return 0;
}

namespace proj_voxel_gi
{
	GLFWwindow* init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
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

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(debug_message_callback, nullptr);

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

	void APIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
	{
		auto source_str = [source]() -> std::string {
			switch (source)
			{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY:  return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER: return "OTHER";
			default: return "UNKNOWN";
			}
		}();

		auto type_str = [type]() {
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR: return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER:  return "MARKER";
			case GL_DEBUG_TYPE_OTHER: return "OTHER";
			default: return "UNKNOWN";
			}
		}();

		auto severity_str = [severity]() {
			switch (severity) {
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
			default: return "UNKNOWN";
			}
		}();

		std::cout << source_str << ", "
			<< type_str << ", "
			<< severity_str << ", "
			<< id << ": "
			<< message << std::endl;
	}
}


