#pragma once

#include "BufferPass.h"

namespace System
{
	class ShadowMapBufferPass : public BufferPass
	{
	public:

		void InitBuffer(const float& screenW, const float& screenH);

		void StartDrawInBuffer();
		void EndDrawInBuffer();

		unsigned int GetShadowMap()
		{
			return m_ShadowMap;
		}

	protected:
		unsigned int m_DepthMapFBO;
		unsigned int m_ShadowMap;
	};
}