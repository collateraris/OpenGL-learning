#include "DepthBufferPass.h"

#include "../../Lessons/3n1_assimp/LoadTexture.h"

using namespace System;

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
	m_BufferShader.Use();
}

void DepthBufferPass::EndDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}