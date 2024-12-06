#pragma once

#include <glm/glm.hpp>

namespace System
{
	class CBox
	{
	public:

		CBox() {};
		~CBox() {};

		void extendBy(const glm::vec3& p);

		const glm::vec3& GetMin() const;

		const glm::vec3& GetMax() const;

	private:

		bool bIsInit = false;

		glm::vec3 mMin;
		glm::vec3 mMax;
	};
}