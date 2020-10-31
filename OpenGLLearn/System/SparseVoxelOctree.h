#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Lessons/1n5_shaders/Shader.h"
#include "../Lessons/3n1_assimp/AssimpData.h"
#include "Buffer.h"

#include <glm/gtx/string_cast.hpp>

#include <vector>

namespace System
{
	class SparseVoxelOctree
	{
	public:
		SparseVoxelOctree();
		~SparseVoxelOctree() = default;

		void Init(const std::vector<lesson_3n1::SMesh>& meshes, size_t depth);

		void Bind() const;

		void Update();

	private:

		lesson_1n5::CShader m_VoxelGenShader;
		lesson_1n5::CShader m_FlagShader;
		lesson_1n5::CShader m_NodeCreationShader;
		lesson_1n5::CShader m_LeafInitShader;
		lesson_1n5::CShader m_MipMapShader;

		Buffer m_VoxelFragmentList{ GL_SHADER_STORAGE_BUFFER };   // vec4
		Buffer m_VoxelCounter{ GL_ATOMIC_COUNTER_BUFFER };        // uint32
		Buffer m_VoxelFragmentColor{ GL_SHADER_STORAGE_BUFFER };  // vec4

		Buffer m_NodePool{ GL_SHADER_STORAGE_BUFFER };        // int32
		Buffer m_NodeCounter{ GL_ATOMIC_COUNTER_BUFFER };     // uint32
		Buffer m_NodeColor{ GL_SHADER_STORAGE_BUFFER };       // vec4

		std::vector<lesson_3n1::SMesh> m_Scene;

		int64_t m_NodeDepth;
		int64_t m_Depth;

		glm::vec3 m_Bmin;
		glm::vec3 m_Bmax;
	};
}