#include "proj_sponza_ogl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../../System/GBufferPass.h"
#include "../../System/SSAO_Pass.h"
#include "../../Lessons/1n9_camera/Camera.h"
#include "../../Lessons/3n1_assimp/LoadTexture.h"
#include "../../Lessons/3n1_assimp/AssimpData.h"
#include "../../Lessons/2n6_multy_lights/LightStates.h"
#include "../../System/Frustum.h"
#include "../../System/Box.h"

using namespace proj_sponza_ogl;

GLfloat g_screenWidth = 800.0f;
GLfloat g_screenHeight = 600.0f;
bool g_blinn = false;

GLFWwindow* init();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

float GetDeltaTime();

void renderSphere();

void renderCube();

void renderQuad();

GLFWwindow* init();

int SponzaScene::lesson_main()
{
	GLFWwindow* window;
	if ((window = init()) == nullptr) return -1;

	lesson_1n5::CShader lightingShader;
	if (!lightingShader.Init("Projects/Sponza/shaders/quad.vs", "Projects/Sponza/shaders/lightingMeshObj.fs")) return -1;

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

	lesson_1n5::CShader quadShader;
	if (!quadShader.Init("Projects/Sponza/shaders/quad.vs", "Projects/Sponza/shaders/quad.fs")) return -1;

	lesson_1n5::CShader depthBufferShader;
	if (!depthBufferShader.Init("Projects/Sponza/shaders/depthBuffer.vs", "Projects/Sponza/shaders/depthBuffer.fs")) return -1;

	System::GBufferPass GBufferRenderPass;
	if (!GBufferRenderPass.LoadShader("Projects/Sponza/shaders/gbuffer.vs", "Projects/Sponza/shaders/gbuffer.fs")) return -1;

	GBufferRenderPass.InitBuffer(g_screenWidth, g_screenHeight);

	System::SSAOPass SSAORenderPass;
	if (!SSAORenderPass.LoadShader("Projects/Sponza/shaders/ssao.vs", "Projects/Sponza/shaders/ssao.fs")) return -1;

	SSAORenderPass.InitBuffer(g_screenWidth, g_screenHeight);

	lesson_3n1::SFileMeshData meshObj;
	lesson_3n1::CLoadAssimpFile::Load("content/model/sponza/sponza.obj", meshObj);

	GLfloat aspectRatio = g_screenWidth / g_screenHeight;

	float nearDist = 0.1f;
	float farDist = 100.f;
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, nearDist, farDist);

	System::CFrustum frustum;

	lightingShader.Use();
	lightingShader.setInt("uIrradianceMap", 0);
	lightingShader.setInt("uPrefilterMap", 1);
	lightingShader.setInt("uBrdfLUT", 2);
	lightingShader.setInt("uShadowMap", 3);
	lightingShader.setInt("uPosition", 4);
	lightingShader.setInt("uNormal", 5);
	lightingShader.setInt("uAlbedo", 6);
	lightingShader.setInt("uRoughnessMetallic", 7);
	lightingShader.setInt("uAO", 8);
	lightingShader.setInt("uNormalInView", 9);

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
	for (const auto& view : captureViews)
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

	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int shadowWidth = 1024, shadowHeight = 1024;
	unsigned int depthMap = lesson_3n1::CLoadTexture::GetDepthMap(shadowWidth, shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float lightNearPlane = 0.1f, lightFarPlane = 90.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, lightNearPlane, lightFarPlane);

	glm::vec3 lightPos = glm::vec3(-18.09f, 21.17f, -3.85f);
	glm::vec3 lightDir = glm::vec3(0.67f, -0.73f, 0.13f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	lesson_1n9::CCamera::Get().SetCameraPosition(lightPos);
	lesson_1n9::CCamera::Get().SetCameraFront(lightDir);

	float deltaTime;
	lesson_3n1::CDrawFileMeshData::Init(meshObj);
	glViewport(0, 0, g_screenWidth, g_screenHeight);

	const std::string uModelStr = "uModel";
	const std::string uProjectionViewStr = "uProjectionView";
	const std::string uProjectionStr = "uProjection";
	const std::string uViewStr = "uView";
	const std::string uInvViewStr = "uInvView";

	SSAORenderPass.GetShader().Use();
	SSAORenderPass.GetShader().setInt("uPosition", 0);
	SSAORenderPass.GetShader().setInt("uNormal", 1);
	SSAORenderPass.GetShader().setInt("uNoise", 2);

	quadShader.Use();
	quadShader.setInt("uTexture", 0);

	while (!glfwWindowShouldClose(window))
	{
		
		deltaTime = GetDeltaTime();
		std::cout << "FPS " << 1.f / deltaTime << std::endl;
		lesson_1n9::CCamera::Get().Movement(deltaTime);

		const glm::mat4 view = lesson_1n9::CCamera::Get().GetView();
		const glm::mat4 inv_view = glm::inverse(view);
		const glm::mat4 proj_view = projection * view;
		
		glm::mat4 model;
		depthBufferShader.Use();
		glm::mat4 lightView = glm::lookAt(lightPos,
			lightDir,
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.));
		glm::mat4 lightSpaceModelMatrix = lightSpaceMatrix * model;
		depthBufferShader.setMatrix4fv("uLightSpaceModelMatrix", lightSpaceModelMatrix);
		///*
		frustum.calculateFrustum(lightProjection, lightView);
		glViewport(0, 0, shadowWidth, shadowHeight);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		for (const lesson_3n1::SMesh& mesh : meshObj.meshes)
		{
			if (frustum.boxInFrustum(mesh.GetMinBB(), mesh.GetMaxBB()))
			{
				lesson_3n1::CDrawFileMeshData::MeshDraw(depthBufferShader, mesh);
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//*/

		glViewport(0, 0, g_screenWidth, g_screenHeight);
		glCullFace(GL_BACK);

		//
		{
			//
			GBufferRenderPass.StartDrawInBuffer();
			GBufferRenderPass.GetShader().setMatrix4fv(uModelStr.c_str(), model);
			SSAORenderPass.GetShader().setMatrix4fv(uViewStr.c_str(), view);
			GBufferRenderPass.GetShader().setMatrix4fv(uProjectionViewStr.c_str(), proj_view);
			frustum.calculateFrustum(projection, view);
			for (const lesson_3n1::SMesh& mesh : meshObj.meshes)
			{
				if (frustum.boxInFrustum(mesh.GetMinBB(), mesh.GetMaxBB()))
				{
					lesson_3n1::CDrawFileMeshData::MeshDraw(GBufferRenderPass.GetShader(), mesh);
				}
			}
			GBufferRenderPass.EndDrawInBuffer();

			//
			
			SSAORenderPass.StartDrawInBuffer();
			SSAORenderPass.GetShader().setMatrix4fv(uProjectionStr.c_str(), projection);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetPositionInViewGBuffer());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetNormalInViewGBuffer());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, SSAORenderPass.GetNoiseTex());
			renderQuad();
			SSAORenderPass.EndDrawInBuffer();

			/*
			quadShader.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SSAORenderPass.GetSSAOColorBuffer());
			renderQuad();
			*/
		}
		///*
		{
			lightingShader.Use();
			lightingShader.setMatrix4fv("uProjectionView", proj_view);
			lightingShader.setMatrix4fv(uInvViewStr.c_str(), inv_view);
			lightingShader.setVec3f("uViewPos", lesson_1n9::CCamera::Get().GetCameraPosition());

			lightingShader.setVec3f("lightPosition", lightPos);
			lightingShader.setVec3f("lightColor", lightColor);
			lightingShader.setVec3f("lightDir", lightDir);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetPositionGBuffer());
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetNormalGBuffer());
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetAlbedoGBuffer());
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetRoughnessMetallic());
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, SSAORenderPass.GetSSAOColorBuffer());
			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetNormalInViewGBuffer());
			lightingShader.setMatrix4fv("uLightSpaceMatrix", lightSpaceModelMatrix);
			renderQuad();
		}
		//*/
		
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


