#pragma once

#include "BufferPass.h"

#include <vector>
#include <string>

namespace System
{
	class SSAOPass : public BufferPass
	{
	public:

		void InitBuffer(const float& screenW, const float& screenH);

		void StartDrawInBuffer();
		void EndDrawInBuffer();


		const glm::vec2& GetNoiseScale() const
		{
			return m_NoiseScale;
		}

		unsigned int GetSSAOColorBuffer() const
		{
			return m_SsaoColorBuffer;
		}

	private:

		unsigned int m_SsaoFBO;
		unsigned int m_SsaoColorBuffer;

	public:

		//unsigned int m_NoiseTexture;
		std::vector<glm::vec3> m_SsaoKernel;

		const int m_KernelSize = 64;
		const float m_Radius = 0.5f;
		const float m_Bias = 0.0025f;

		std::vector<std::string> m_KernelUniforms;
		const std::string m_KernelSizeUniformStr = "uKernelSize";
		const std::string m_NoiseScaleUniformStr = "uNoiseScale";
		const std::string m_RadiusUniformStr = "uRadius";
		const std::string m_BiasUniformStr = "uBias";

		glm::vec2 m_NoiseScale;
	};

	class SSAOBlurPass : public BufferPass
	{
	public:

		void InitBuffer(const GLfloat& screenW, const GLfloat& screenH);

		void StartDrawInBuffer();
		void EndDrawInBuffer();

		unsigned int GetSSAOColorBlurBuffer() const
		{
			return m_SsaoColorBufferBlur;
		}

	private:

		unsigned int m_SsaoBlurFBO;
		unsigned int m_SsaoColorBufferBlur;

	};
}