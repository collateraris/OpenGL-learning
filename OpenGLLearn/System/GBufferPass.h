#pragma once

#include "BufferPass.h"

namespace System
{
	class GBufferPass : public BufferPass
	{
	public:
		void InitBuffer(const float& screenW, const float& screenH);

		void StartDrawInBuffer();
		void EndDrawInBuffer();

		unsigned int GetPositionGBuffer() const
		{
			return m_PositionGBuffer;
		}

		unsigned int GetNormalGBuffer() const
		{
			return m_NormalGBuffer;
		}

		unsigned int GetAlbedoGBuffer() const
		{
			return m_AlbedoGBuffer;
		}

		unsigned int GetRoughnessMetallic() const
		{
			return m_RoughnessMetallic;
		}

	private:
		unsigned int m_GBuffer;

		unsigned int m_PositionGBuffer;
		unsigned int m_NormalGBuffer;
		unsigned int m_AlbedoGBuffer;
		unsigned int m_RoughnessMetallic;
	};
}