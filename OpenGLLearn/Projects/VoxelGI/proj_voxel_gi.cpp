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

#include "../../System/DepthBufferPass.h"
#include "../../System/ShadowMapBufferPass.h"
#include "../../System/GBufferPass.h"
#include "../../System/SSAO_Pass.h"
#include "../../Lessons/1n9_camera/Camera.h"
#include "../../Lessons/3n1_assimp/LoadTexture.h"
#include "../../Lessons/3n1_assimp/AssimpData.h"
#include "../../Lessons/2n6_multy_lights/LightStates.h"
#include "../../System/Frustum.h"
#include "../../System/Box.h"
#include "../../System/SparseVoxelOctree.h"

namespace proj_voxel_gi
{
	GLfloat g_screenWidth = 1200.0f;
	GLfloat g_screenHeight = 800.0f;
	const unsigned int shadowWidth = 1024, shadowHeight = 1024;

	enum class EDemoState
	{
		SSAO,
		PBRwithAO,
		PBRwithoutAO,
		DepthMap,
		ShadowMap,
	};

	EDemoState g_demoState = EDemoState::SSAO;


	GLFWwindow* init();

	void APIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	float GetDeltaTime();

	void renderSphere();

	void renderCube();

	void renderQuad();

	GLFWwindow* init();
}

using namespace proj_voxel_gi;

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

	lesson_1n5::CShader ssaoDebugShader;
	if (!ssaoDebugShader.Init("Projects/Sponza/shaders/quad.vs", "Projects/Sponza/shaders/quad.fs")) return -1;

	System::DepthBufferPass depthBufferRenderPass;
	if (!depthBufferRenderPass.LoadShader("Projects/Sponza/shaders/depthBuffer.vs", "Projects/Sponza/shaders/depthBuffer.fs")) return -1;

	depthBufferRenderPass.InitBuffer(g_screenWidth, g_screenHeight);

	System::ShadowMapBufferPass shadowMapRenderPass;
	if (!shadowMapRenderPass.LoadShader("Projects/Sponza/shaders/shadowMapBuffer.vs", "Projects/Sponza/shaders/depthBuffer.fs")) return -1;
	shadowMapRenderPass.InitBuffer(shadowWidth, shadowHeight);

	System::GBufferPass GBufferRenderPass;
	if (!GBufferRenderPass.LoadShader("Projects/Sponza/shaders/gbuffer.vs", "Projects/Sponza/shaders/gbuffer.fs")) return -1;

	GBufferRenderPass.InitBuffer(g_screenWidth, g_screenHeight);

	System::SSAOPass SSAORenderPass;
	if (!SSAORenderPass.LoadShader("Projects/Sponza/shaders/ssao.vs", "Projects/Sponza/shaders/ssao.fs")) return -1;

	SSAORenderPass.InitBuffer(g_screenWidth, g_screenHeight);

	lesson_3n1::SFileMeshData sponzaScene;
	lesson_3n1::CLoadAssimpFile::Load("content/model/sponza/sponza.obj", sponzaScene);

	GLfloat aspectRatio = g_screenWidth / g_screenHeight;

	float nearDist = 0.1f;
	float farDist = 1000.f;
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(lesson_1n9::CCamera::Get().GetFov()), aspectRatio, nearDist, farDist);

	float lightNearPlane = 0.1f, lightFarPlane = 90.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, lightNearPlane, lightFarPlane);

	System::CFrustum frustum;

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

	glm::vec3 lightPos = glm::vec3(-18.09f, 21.17f, -10.85f);
	glm::vec3 lightDir = glm::vec3(0.67f, -0.73f, 0.13f);
	glm::vec3 lightColor = glm::vec3(800.0f, 800.0f, 800.0f);

	lesson_1n9::CCamera::Get().SetCameraPosition(lightPos);
	lesson_1n9::CCamera::Get().SetCameraFront(lightDir);

	float deltaTime;
	lesson_3n1::CDrawFileMeshData::Init(sponzaScene);
	System::SparseVoxelOctree octree;
	octree.Init(sponzaScene.meshes, 5);

	unsigned int fogNoiseTex = lesson_3n1::CLoadTexture::GetTexture(g_screenWidth, g_screenHeight);

	const std::string uModelStr = "uModel";
	const std::string uProjectionViewStr = "uProjectionView";
	const std::string uMVPStr = "uMVP";
	const std::string uProjectionStr = "uProjection";
	const std::string uViewStr = "uView";
	const std::string uViewPosStr = "uViewPos";
	const std::string uInvViewStr = "uInvView";
	const std::string uLightSpaceModelStr = "uLightSpaceModel";
	const std::string uLightPositionStr = "uLightPosition";
	const std::string uLightColorStr = "uLightColor";
	const std::string uLightDirStr = "uLightDir";
	const std::string uAspectRatioStr = "uAspectRatio";
	const std::string uTanHalfFOVStr = "uTanHalfFOV";
	const std::string uAOIncludeStr = "uAOInclude";
	const std::string uFogNoiseStr = "uFogNoise";

	SSAORenderPass.GetShader().Use();
	SSAORenderPass.GetShader().setInt("uDepthMap", 0);
	SSAORenderPass.GetShader().setFloat(uAspectRatioStr.c_str(), aspectRatio);
	SSAORenderPass.GetShader().setFloat(uTanHalfFOVStr.c_str(), glm::tan(glm::radians(lesson_1n9::CCamera::Get().GetFov())));

	ssaoDebugShader.Use();
	ssaoDebugShader.setInt("uTexture", 0);

	lightingShader.Use();
	lightingShader.setInt("uIrradianceMap", 0);
	lightingShader.setInt("uPrefilterMap", 1);
	lightingShader.setInt("uBrdfLUT", 2);
	lightingShader.setInt("uPosition", 3);
	lightingShader.setInt("uNormal", 4);
	lightingShader.setInt("uAlbedo", 5);
	lightingShader.setInt("uRoughnessMetallic", 6);
	lightingShader.setInt("uAO", 7);
	lightingShader.setInt("uShadowMap", 8); 
	lightingShader.setInt(uFogNoiseStr.c_str(), 9);

	SSAORenderPass.GetShader().Use();
	auto& ssaoShaderRef = SSAORenderPass.GetShader();
	for (unsigned int i = 0; i < SSAORenderPass.m_KernelSize; ++i)
		ssaoShaderRef.setVec3f(SSAORenderPass.m_KernelUniforms[i].c_str(), SSAORenderPass.m_SsaoKernel[i]);

	ssaoShaderRef.setInt(SSAORenderPass.m_KernelSizeUniformStr.c_str(), SSAORenderPass.m_KernelSize);
	ssaoShaderRef.setFloat(SSAORenderPass.m_RadiusUniformStr.c_str(), SSAORenderPass.m_Radius);
	ssaoShaderRef.setFloat(SSAORenderPass.m_BiasUniformStr.c_str(), SSAORenderPass.m_Bias);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.));
	const glm::mat4 lightView = glm::lookAt(lightPos,
		lightDir,
		glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 lightSpaceMatrix = lightProjection * lightView * model;

	frustum.calculateFrustum(lightProjection, lightView);
	glViewport(0, 0, shadowWidth, shadowHeight);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	shadowMapRenderPass.StartDrawInBuffer();
	shadowMapRenderPass.GetShader().setMatrix4fv(uLightSpaceModelStr.c_str(), lightSpaceMatrix);
	for (const lesson_3n1::SMesh& mesh : sponzaScene.meshes)
	{
		if (frustum.boxInFrustum(mesh.GetMinBB(), mesh.GetMaxBB()))
		{
			lesson_3n1::CDrawFileMeshData::MeshDraw(shadowMapRenderPass.GetShader(), mesh);
		}
	}
	shadowMapRenderPass.EndDrawInBuffer();

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
		const glm::mat4 proj_view = projection * view;
		const glm::mat4 mvp = proj_view;// * model;

		frustum.calculateFrustum(projection, view);

		depthBufferRenderPass.StartDrawInBuffer();
		depthBufferRenderPass.GetShader().setMatrix4fv(uMVPStr.c_str(), mvp);
		for (const lesson_3n1::SMesh& mesh : sponzaScene.meshes)
		{
			if (frustum.boxInFrustum(mesh.GetMinBB(), mesh.GetMaxBB()))
			{
				lesson_3n1::CDrawFileMeshData::MeshDraw(depthBufferRenderPass.GetShader(), mesh);
			}
		}
		depthBufferRenderPass.EndDrawInBuffer();

		//
		{
			
			GBufferRenderPass.StartDrawInBuffer();
			GBufferRenderPass.GetShader().setMatrix4fv(uModelStr.c_str(), model);
			SSAORenderPass.GetShader().setMatrix4fv(uViewStr.c_str(), view);
			GBufferRenderPass.GetShader().setMatrix4fv(uProjectionViewStr.c_str(), proj_view);
			for (const lesson_3n1::SMesh& mesh : sponzaScene.meshes)
			{
				if (frustum.boxInFrustum(mesh.GetMinBB(), mesh.GetMaxBB()))
				{
					lesson_3n1::CDrawFileMeshData::MeshDraw(GBufferRenderPass.GetShader(), mesh);
				}
			}
			GBufferRenderPass.EndDrawInBuffer();

			SSAORenderPass.StartDrawInBuffer();
			SSAORenderPass.GetShader().setMatrix4fv(uProjectionStr.c_str(), projection);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthBufferRenderPass.GetDepthMap());
			renderQuad();
			SSAORenderPass.EndDrawInBuffer();
				
		}

		bool bAOInclude = true;

		switch (g_demoState)
		{
		case EDemoState::SSAO:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ssaoDebugShader.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SSAORenderPass.GetSSAOColorBuffer());
			renderQuad();
			break;
		case EDemoState::DepthMap:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ssaoDebugShader.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthBufferRenderPass.GetDepthMap());
			renderQuad();
			break;
		case EDemoState::ShadowMap:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ssaoDebugShader.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shadowMapRenderPass.GetShadowMap());
			renderQuad();
			break;
		case EDemoState::PBRwithoutAO:
			bAOInclude = false;
		case EDemoState::PBRwithAO:
		{
			lightingShader.Use();
			lightingShader.setInt(uAOIncludeStr.c_str(), bAOInclude);
			lightingShader.setMatrix4fv(uProjectionViewStr.c_str(), proj_view);
			lightingShader.setMatrix4fv(uLightSpaceModelStr.c_str(), lightSpaceMatrix);
			lightingShader.setVec3f(uViewPosStr.c_str(), lesson_1n9::CCamera::Get().GetCameraPosition());

			lightingShader.setVec3f(uLightPositionStr.c_str(), lightPos);
			lightingShader.setVec3f(uLightColorStr.c_str(), lightColor);
			lightingShader.setVec3f(uLightDirStr.c_str(), lightDir);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetPositionGBuffer());
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetNormalGBuffer());
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetAlbedoGBuffer());
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, GBufferRenderPass.GetRoughnessMetallic());
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, SSAORenderPass.GetSSAOColorBuffer());
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, shadowMapRenderPass.GetShadowMap());
			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, fogNoiseTex);
			renderQuad();
		}
			break;
		default:
			break;
		}

		
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

		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_1:
				g_demoState = EDemoState::SSAO;
				break;
			case GLFW_KEY_2:
				g_demoState = EDemoState::PBRwithAO;
				break;
			case GLFW_KEY_3:
				g_demoState = EDemoState::PBRwithoutAO;
				break;
			case GLFW_KEY_4:
				g_demoState = EDemoState::DepthMap;
				break;
			case GLFW_KEY_5:
				g_demoState = EDemoState::ShadowMap;
				break;
			default:
				break;
			}
		}


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


