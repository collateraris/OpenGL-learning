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
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / m_KernelSize;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_SsaoKernel.push_back(sample);
	}

	for (unsigned int i = 0; i < m_KernelSize; ++i)
	{
		m_KernelUniforms.push_back("uSamples[" + std::to_string(i) + "]");
	}

	// generate noise texture
	// ----------------------
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		m_SsaoNoise.push_back(noise);
	}

	glGenTextures(1, &m_NoiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_NoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &m_SsaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_NoiseScale = {screenW / 4, screenH / 4};

}

void SSAOPass::StartDrawInBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_SsaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	m_BufferShader.Use();

	// Send kernel + rotation 
	for (unsigned int i = 0; i < m_KernelSize; ++i)
		m_BufferShader.setVec3f(m_KernelUniforms[i].c_str(), m_SsaoKernel[i]);

	m_BufferShader.setVec2f(m_NoiseScaleUniformStr.c_str(), m_NoiseScale);
	m_BufferShader.setInt(m_KernelSizeUniformStr.c_str(), m_KernelSize);
	m_BufferShader.setFloat(m_RadiusUniformStr.c_str(), m_Radius);
	m_BufferShader.setFloat(m_BiasUniformStr.c_str(), m_Bias);

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