#include "GBufferPass.h"

#include "../../Lessons/1n5_shaders/Shader.h"

#include <vector>

using namespace System;

void GBufferPass::InitBuffer(const float& screenW, const float& screenH)
{
	glGenFramebuffers(1, &m_GBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);

	glGenTextures(1, &m_PositionGBuffer);
	glBindTexture(GL_TEXTURE_2D, m_PositionGBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenW, screenH, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PositionGBuffer, 0);

	glGenTextures(1, &m_NormalGBuffer);
	glBindTexture(GL_TEXTURE_2D, m_NormalGBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenW, screenH, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_NormalGBuffer, 0);


	glGenTextures(1, &m_AlbedoGBuffer);
	glBindTexture(GL_TEXTURE_2D, m_AlbedoGBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_AlbedoGBuffer, 0);

	glGenTextures(1, &m_RoughnessMetallic);
	glBindTexture(GL_TEXTURE_2D, m_RoughnessMetallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, screenW, screenH, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_RoughnessMetallic, 0);

	std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
		GL_COLOR_ATTACHMENT3};
	glDrawBuffers(attachments.size(), attachments.data());

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenW, screenH);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBufferPass::StartDrawInBuffer()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_BufferShader.Use();
}

void GBufferPass::EndDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}