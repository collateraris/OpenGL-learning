#include "DepthBufferPass.h"

#include "../../Lessons/3n1_assimp/LoadTexture.h"
#include "StringConst.h"

#include <cassert>

using namespace System;

DepthBufferPass::DepthBufferPass(const DepthBufferPassInfo& info)
{
	bool result;
	//result = mWriteInBufferShader.Init("System/shaders/DepthBuffer/depthBuffer.vert", "System/shaders/DepthBuffer/depthBuffer.frag");

	//assert(result);

	result = mDepthDebugShader.Init("System/shaders/DepthBuffer/depthBufferDebug.vert", "System/shaders/DepthBuffer/depthBufferDebug.frag");

	assert(result);

	mDepthDebugShader.Use();
	mDepthDebugShader.setInt(uDepthTexStr.c_str(), 0);
	mDepthDebugShader.setFloat(uFarStr.c_str(), info.screenFar);
	mDepthDebugShader.setFloat(uNearStr.c_str(), info.screenNear);
}

void DepthBufferPass::InitBuffer(const float& screenW, const float& screenH)
{
	glGenFramebuffers(1, &m_DepthMapFBO);

	m_DepthMap = lesson_3n1::CLoadTexture::GetDepthMap(screenW, screenH);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthBufferPass::StartDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	m_BufferShader.Use();
}

void DepthBufferPass::EndDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void DepthBufferPass::SetMVP(const glm::mat4& mvp)
{
	m_BufferShader.setMatrix4fv(uMVPStr.c_str(), mvp);
}

void DepthBufferPass::Debug()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mDepthDebugShader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
}