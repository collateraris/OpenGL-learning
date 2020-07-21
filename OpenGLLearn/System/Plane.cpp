#include "Plane.h"

using namespace System;

glm::vec4 CPlane::GetPlane(const glm::vec3& s1, const glm::vec3& s2, const glm::vec3& s3)
{
	float A = s2.x * (s3.y - s3.z)
		+ s2.y * (s3.z - s3.x)
		+ s2.z * (s3.x - s3.y);

	float B = s3.x * (s1.y - s1.z)
		+ s3.y * (s1.z - s1.x)
		+ s3.z * (s1.x - s1.y);

	float C = s1.x * (s2.y - s2.z)
		+ s1.y * (s2.z - s2.x)
		+ s1.z * (s2.x - s2.y);

	float D = -( s1.x * (s2.y * s3.z - s2.z * s3.y)
		+ s1.y * (s2.z * s3.x - s2.x * s3.z)
		+ s1.z * (s2.x * s3.y - s2.y * s3.z));

	return {A, B, C, D};
}