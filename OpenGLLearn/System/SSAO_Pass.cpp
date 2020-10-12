#include "SSAO_Pass.h"

#include "../../Lessons/1n5_shaders/Shader.h"

#include <cassert>
#include <random>
#include <iostream>

using namespace System;

void SSAOPass::InitBuffer(const float& screenW, const float& screenH)
{
	glGenFramebuffers(1, &m_SsaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_SsaoFBO);

	glGenTextures(1, &m_SsaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_SsaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenW, screenH, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SsaoColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// generate sample kernel
	// ----------------------
	auto lerp = [](float a, float b, float f)
	{
		return a + f * (b - a);
	};

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	for (unsigned int i = 0; i < m_KernelSize; ++i)
	{
		float scale = static_cast<float>(i) / m_KernelSize;
		glm::vec3 v;
		v.x = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.y = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.z = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		// Use an acceleration function so more points are
		// located closer to the origin
		v *= (0.1f + 0.9f * scale * scale);
		m_SsaoKernel.emplace_back(v);
		/*
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / m_KernelSize;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_SsaoKernel.push_back(sample);
		*/
	}

	for (unsigned int i = 0; i < m_KernelSize; ++i)
	{
		m_KernelUniforms.push_back("uSamples[" + std::to_string(i) + "]");
	}

}

void SSAOPass::StartDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_SsaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	m_BufferShader.Use();
}

void SSAOPass::EndDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOBlurPass::InitBuffer(const GLfloat& screenW, const GLfloat& screenH)
{
	glGenFramebuffers(1, &m_SsaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_SsaoBlurFBO);
	glGenTextures(1, &m_SsaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, m_SsaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenW, screenH, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SsaoColorBufferBlur, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void SSAOBlurPass::StartDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_SsaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	m_BufferShader.Use();
}

void SSAOBlurPass::EndDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}