#include "Box.h"

using namespace System;

void CBox::extendBy(const glm::vec3& p)
{
	if (!bIsInit)
	{
		mMin = mMax = p;
		bIsInit = true;
		return;
	}

	float pX = p.x;
	float pY = p.y;
	float pZ = p.z;

	float minX = pX < mMin.x ? pX : mMin.x;
	float minY = pY < mMin.y? pY : mMin.y;
	float minZ = pZ < mMin.z ? pZ : mMin.z;
	mMin = glm::vec3(minX, minY, minZ);

	float maxX = pX > mMax.x ? pX : mMax.x;
	float maxY = pY > mMax.y ? pY : mMax.y;
	float maxZ = pZ > mMax.z ? pZ : mMax.z;
	mMax = glm::vec3(maxX, maxY, maxZ);

}

const glm::vec3& CBox::GetMin() const
{
	return mMin;
}

const glm::vec3& CBox::GetMax() const
{
	return mMax;
}