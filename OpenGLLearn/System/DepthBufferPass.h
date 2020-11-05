#pragma once

#include "BufferPass.h"
#include "../Lessons/1n5_shaders/Shader.h"

namespace System
{
	struct DepthBufferPassInfo
	{
		float screenFar = 1000.0f;
		float screenNear = 0.1f;
	};

	class DepthBufferPass : public BufferPass
	{
	public:

		DepthBufferPass() {};
		DepthBufferPass(const DepthBufferPassInfo& info);

		void InitBuffer(const float& screenW, const float& screenH);

		void StartDrawInBuffer();
		void EndDrawInBuffer();
		void Debug();

		unsigned int GetDepthMap()
		{
			return m_DepthMap;
		}

		void SetMVP(const glm::mat4& mvp);

	protected:

		unsigned int m_DepthMapFBO;
		unsigned int m_DepthMap;

	private:

		lesson_1n5::CShader mDepthDebugShader;
	};
}