#include "VoxelGrid.h"

#include "StringConst.h"
#include <cassert>

using namespace System;

VoxelGrid::VoxelGrid(const VoxelGridInfo& info)
{
	m_Info = info;

	{
		bool result;
		result = m_VoxelGridFillShader.Init("System/shaders/VoxelGrid/voxelGridFill.vert", "System/shaders/VoxelGrid/voxelGridFill.frag",
			"System/shaders/VoxelGrid/voxelGridFill.geom");

		assert(result);

		result = m_VoxelGridVisibleShader.Init("System/shaders/VoxelGrid/voxelGridVisible.vert", "System/shaders/VoxelGrid/voxelGridVisible.frag");

		assert(result);

		m_VoxelGridVisibleShader.Use();
		m_VoxelGridVisibleShader.setInt(uDepthTexStr.c_str(), 0);
	}

	{
		int voxelsNum = info.voxelGridNum * info.voxelGridNum * info.voxelGridNum;
		m_VoxelGridBufferSize = voxelsNum * sizeof(VoxelGrid::VOXEL);
		std::vector<VOXEL> voxels(voxelsNum, VOXEL{});
		m_VoxelGridBuffer.SetStorage(voxels, GL_DYNAMIC_STORAGE_BIT);
	}

	{
		auto& gridData = m_GridBufferData[0];
		gridData.gridCellSizes = info.gridHalfExtent * 2.0f / info.voxelGridNum;
		gridData.invGridCellSizes = 1.0 / gridData.gridCellSizes;

		float gridHalfExtent = info.gridHalfExtent;
		m_GridProjMatrix = glm::ortho(-gridHalfExtent, gridHalfExtent,
			-gridHalfExtent, gridHalfExtent,
			0.0f, 2.0f * gridHalfExtent);

		m_GridParamsBufferSize = sizeof(VoxelGrid::GRID_PARAMS);
		m_GridParamsBuffer.SetStorage(m_GridBufferData, GL_DYNAMIC_STORAGE_BIT);
	}
}

VoxelGrid::~VoxelGrid()
{
}

void VoxelGrid::UpdateGrid(lesson_1n9::CCamera& camera)
{
	const auto& camPos = camera.GetCameraPosition();
	const auto& camDir = camera.GetCameraDirection();
	glm::vec3 tmpGridPosition = camPos + camDir * 2.0f * m_Info.gridHalfExtent;

	auto& gridData = m_GridBufferData[0];
	gridData.snappedGridPositions = tmpGridPosition;
	gridData.snappedGridPositions *= gridData.gridCellSizes;
	glm::floor(gridData.snappedGridPositions);
	gridData.snappedGridPositions *= gridData.gridCellSizes;

	// back to front viewProjMatrix
	{
		glm::vec3 translate = -gridData.snappedGridPositions - glm::vec3(0.0f, 0.0f, m_Info.gridHalfExtent);
		glm::mat4 gridTranslateMatrix = glm::translate(m_IdentityMatrix, translate);
		gridData.gridViewProjMatrices[0] =  m_GridProjMatrix * gridTranslateMatrix;
	}

	// right to left viewProjMatrix
	{
		glm::vec3 translate = -gridData.snappedGridPositions - glm::vec3(m_Info.gridHalfExtent, 0.0f, 0.0f);
		glm::mat4 gridTranslateMatrix = glm::rotate(m_IdentityMatrix, -90.0f, translate);
		gridData.gridViewProjMatrices[1] = m_GridProjMatrix * gridTranslateMatrix;
	}

	// top to down viewProjMatrix
	{
		glm::vec3 translate = -gridData.snappedGridPositions - glm::vec3(0.0f, m_Info.gridHalfExtent, 0.0f);
		glm::mat4 gridTranslateMatrix = glm::rotate(m_IdentityMatrix, 90.0f, translate);
		gridData.gridViewProjMatrices[2] = m_GridProjMatrix * gridTranslateMatrix;
	}

	m_GridParamsBuffer.BindBase(0);
	m_VoxelGridBuffer.BindBase(1);

	glClearNamedBufferSubData(m_VoxelGridBuffer.GetHandle(), GL_RGBA32F, 0, m_VoxelGridBufferSize, GL_RGBA, GL_FLOAT, &m_ClearVoxelVal);

	m_GridParamsBuffer.SetContentSubData(m_GridBufferData, 0);

	glDisable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_VoxelGridFillShader.Use();

}

void VoxelGrid::Debug(const glm::mat4& invViewProjMatrix)
{
	m_GridParamsBuffer.BindBase(0);
	m_VoxelGridBuffer.BindBase(1);

	m_VoxelGridVisibleShader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Info.depthMapId);
	m_VoxelGridVisibleShader.setMatrix4fv(uInvViewProjMatrixStr.c_str(), invViewProjMatrix);
}

