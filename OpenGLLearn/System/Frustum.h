#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace System
{
	class CFrustum
	{
	public:

		CFrustum();
		CFrustum(CFrustum&) = delete;
		void operator=(CFrustum&) = delete;
		CFrustum(CFrustum&&) = delete;
		void operator=(CFrustum&&) = delete;

		void calculateFrustum(const glm::mat4& projection, const glm::mat4& view);

		bool boxInFrustum(const glm::vec3& min, const glm::vec3& max);

	private:
		enum EFrustumSide : int 
		{
			TOP = 0, 
			BOTTOM, 
			LEFT,
			RIGHT, 
			BACK, 
			FRONT,
			MAX
		};

		std::vector<glm::vec4> mFrustumPlanes;
	};
}