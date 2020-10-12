#pragma once

#include "BufferPass.h"

namespace System
{
	class DepthBufferPass : public BufferPass
	{
	public:

		void InitBuffer(const float& screenW, const float& screenH);

		void StartDrawInBuffer();
		void EndDrawInBuffer();

		unsigned int GetDepthMap()
		{
			return m_DepthMap;
		}

	protected:
		unsigned int m_DepthMapFBO;
		unsigned int m_DepthMap;
	};
}