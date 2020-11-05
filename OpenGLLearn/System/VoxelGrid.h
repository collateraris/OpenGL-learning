#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Lessons/1n5_shaders/Shader.h"
#include "../Lessons/3n1_assimp/AssimpData.h"
#include "../../Lessons/1n9_camera/Camera.h"
#include "Buffer.h"

#include <glm/gtx/string_cast.hpp>

#include <vector>

namespace System
{
	struct VoxelGridInfo
	{
		VoxelGridInfo() {};
		void operator=(const VoxelGridInfo& info)
		{
			this->depthMapId = info.depthMapId;
		}

		const int voxelGridNum = 32;
		const float gridHalfExtent = 1000.0f;

		unsigned int depthMapId = 0;
	};

	class VoxelGrid
	{
	public:
		VoxelGrid() {};

		VoxelGrid(const VoxelGridInfo& info);

		~VoxelGrid();

		//__declspec(align(sizeof(glm::mat4))) 
		struct GRID_PARAMS
		{
			glm::mat4 gridViewProjMatrices[3];  // viewProjMatrices for generating the voxel-grids
			glm::vec4 gridPositions;            // center of FINE_GRID/
			glm::vec4 globalIllumParams;        // x = flux amplifier, y = occlusion amplifier, z = diffuse GI-contribution power
			glm::vec3 snappedGridPositions;     // center of FINE_GRID, snapped to the corresponding grid-cell extents 
			float gridCellSizes;            // (inverse) sizes of grid-cells FINE_GRID
			float invGridCellSizes;
		};

		//__declspec(align(sizeof(glm::uvec4))) 
		struct VOXEL
		{
			glm::uvec4 normalMasks = {0, 0, 0, 0}; // encoded normals
			glm::uint colorMask = 0;	// encoded color
			glm::uint occlusion = 0;	// voxel only contains geometry info if occlusion > 0
		};

		void UpdateGrid(lesson_1n9::CCamera& camera);

		lesson_1n5::CShader& GetVoxelGridFillShader()
		{
			return m_VoxelGridFillShader;
		}

		void Debug(const glm::mat4& invViewProjMatrix);

		const glm::mat4& GetProj() const
		{
			return m_GridProjMatrix;
		}

	private:

		Buffer m_GridParamsBuffer{ GL_SHADER_STORAGE_BUFFER };
		Buffer m_VoxelGridBuffer{ GL_SHADER_STORAGE_BUFFER };

		size_t m_GridParamsBufferSize = 0;
		size_t m_VoxelGridBufferSize = 0;

		std::vector<GRID_PARAMS> m_GridBufferData = { GRID_PARAMS {} };

		VoxelGridInfo m_Info;

		glm::mat4 m_GridProjMatrix;
		const glm::mat4 m_IdentityMatrix = glm::mat4(1);
		const VOXEL m_ClearVoxelVal = {};

		lesson_1n5::CShader m_VoxelGridFillShader;
		lesson_1n5::CShader m_VoxelGridVisibleShader;
	};
}