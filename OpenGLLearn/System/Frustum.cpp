#include "Frustum.h"

#include "Plane.h"
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

using namespace System;

CFrustum::CFrustum()
{
	mFrustumPlanes.resize(EFrustumSide::MAX, {});
}

void CFrustum::calculateFrustum(const glm::mat4& projection, const glm::mat4& view)
{
	glm::mat4 clipMatrix = view * projection;
	float clip[16] = { 0.0 };

	const float* pSource = (const float*)glm::value_ptr(clipMatrix);
	for (int i = 0; i < 16; ++i)
		clip[i] = pSource[i];


	mFrustumPlanes[EFrustumSide::RIGHT] = glm::normalize(
		glm::vec4(
			clip[3] - clip[0], 
			clip[7] - clip[4],
			clip[11] - clip[8],
			clip[15] - clip[12]));

	mFrustumPlanes[EFrustumSide::LEFT] = glm::normalize(
		glm::vec4(
			clip[3] + clip[0],
			clip[7] + clip[4],
			clip[11] + clip[8],
			clip[15] + clip[12]));

	mFrustumPlanes[EFrustumSide::BOTTOM] = glm::normalize(
		glm::vec4(
			clip[3] + clip[1],
			clip[7] + clip[5],
			clip[11] + clip[9],
			clip[15] + clip[13]));

	mFrustumPlanes[EFrustumSide::TOP] = glm::normalize(
		glm::vec4(
			clip[3] - clip[1],
			clip[7] - clip[5],
			clip[11] - clip[9],
			clip[15] - clip[13]));

	mFrustumPlanes[EFrustumSide::BACK] = glm::normalize(
		glm::vec4(
			clip[3] - clip[2],
			clip[7] - clip[6],
			clip[11] - clip[10],
			clip[15] - clip[14]));

	mFrustumPlanes[EFrustumSide::FRONT] = glm::normalize(
		glm::vec4(
			clip[3] + clip[2],
			clip[7] + clip[6],
			clip[11] + clip[10],
			clip[15] + clip[14]));
			
}

bool CFrustum::boxInFrustum(const glm::vec3& Min, const glm::vec3& Max)
{
	bool inside = true;
	// for each plane do ...
	for (int i = 0; i < EFrustumSide::MAX; i++)
	{
		float d = std::max(Min.x * mFrustumPlanes[i].x, Max.x * mFrustumPlanes[i].x)
			+ std::max(Min.y * mFrustumPlanes[i].y, Max.y * mFrustumPlanes[i].y)
			+ std::max(Min.z * mFrustumPlanes[i].z, Max.z * mFrustumPlanes[i].z)
			+ mFrustumPlanes[i].w;
		inside &= d > 0;
	}

	return inside;
}
