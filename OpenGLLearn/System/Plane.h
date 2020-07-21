#pragma once

#include <glm/glm.hpp>

namespace System
{
	class CPlane
	{
	public:

		CPlane() = default;
		~CPlane() = default;
		CPlane(CPlane&) = delete;
		void operator=(CPlane&) = delete;
		CPlane(CPlane&&) = delete;
		void operator=(CPlane&&) = delete;

		static glm::vec4 GetPlane(const glm::vec3& s1, const glm::vec3& s2, const glm::vec3& s3);

	private:

	};
}