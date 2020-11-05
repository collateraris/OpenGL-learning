#include "SparseVoxelOctree.h"

#include "StringConst.h"

#include <glm/gtx/component_wise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

using namespace System;

SparseVoxelOctree::SparseVoxelOctree()
{
	bool result;
	result = m_VoxelGenShader.Init("System/shaders/SparseVoxelOctree/vexelGen.vert",
		"System/shaders/SparseVoxelOctree/vexelGen.frag",
		"System/shaders/SparseVoxelOctree/vexelGen.geom");
	assert(result);

	result = m_NodeCreationShader.Init("System/shaders/SparseVoxelOctree/createNodes.comp");
	assert(result);

	result = m_FlagShader.Init("System/shaders/SparseVoxelOctree/flagNodes.comp");
	assert(result);

	result = m_LeafInitShader.Init("System/shaders/SparseVoxelOctree/initLeafNodes.comp");
	assert(result);

	result = m_MipMapShader.Init("System/shaders/SparseVoxelOctree/mipMapNodes.comp");
	assert(result);
}

void SparseVoxelOctree::Init(const std::vector<lesson_3n1::SMesh>& meshes, size_t depth)
{
	m_NodeDepth = static_cast<int64_t>(glm::pow(2, depth));
	m_Depth = static_cast<int64_t>(depth);

	std::copy(meshes.begin(), meshes.end(),
		std::back_inserter(m_Scene));


	size_t triangleNum = 0;
	
	std::for_each(m_Scene.begin(), m_Scene.end(), [&triangleNum](auto& mesh) {
		triangleNum += mesh.GetIndices().size();
	});
	triangleNum /= 3;
	
	
	{
		std::vector<glm::vec4> fragmentsNum(triangleNum * 4, glm::vec4(0.f));
		m_VoxelFragmentListBufferSize = fragmentsNum.size() * sizeof(glm::vec4);
		m_VoxelFragmentColorBufferSize = m_VoxelFragmentListBufferSize;
		m_VoxelFragmentList.SetStorage(fragmentsNum, GL_DYNAMIC_STORAGE_BIT);
		m_VoxelFragmentColor.SetStorage(fragmentsNum, GL_DYNAMIC_STORAGE_BIT);

		const int powNodeDepth3 = static_cast<int>(std::pow(m_NodeDepth, 3));

		m_NodePoolBufferSize = powNodeDepth3 * sizeof(GLint);
		m_NodeColorBufferSize = powNodeDepth3 * 2 * sizeof(glm::vec4);
		m_NodePool.SetStorage(std::vector<GLint>(powNodeDepth3, -1), GL_DYNAMIC_STORAGE_BIT);
		m_NodeColor.SetStorage(std::vector<glm::vec4>(powNodeDepth3 * 2, glm::vec4(0.f)), GL_DYNAMIC_STORAGE_BIT);

		m_VoxelCounter.SetStorage(std::vector<GLuint>{ 0u }, GL_DYNAMIC_STORAGE_BIT);
		m_NodeCounter.SetStorage(std::vector<GLuint>{ 1u }, GL_DYNAMIC_STORAGE_BIT);
	}

	{
		m_Bmin = glm::vec3(std::numeric_limits<float>::max());
		m_Bmax = glm::vec3(std::numeric_limits<float>::lowest());
		std::for_each(m_Scene.begin(), m_Scene.end(), [&](auto& mesh)
		{
			const glm::vec3& meshBMin = mesh.GetMinBB();
			const glm::vec3& meshBMax = mesh.GetMaxBB();
			m_Bmin = glm::min(m_Bmin, meshBMin);
			m_Bmax = glm::max(m_Bmax, meshBMax);
		});

		const glm::vec3 c = (m_Bmin + m_Bmax) / 2.0f;
		const float halfSize = glm::compMax(m_Bmax - m_Bmin) / 2.0f;
		m_Bmin = c - halfSize;
		m_Bmax = c + halfSize;
	}

	glm::mat4 Proj = glm::ortho(
		m_Bmin.x, m_Bmax.x,
		m_Bmin.y, m_Bmax.y,
		-m_Bmax.z, -m_Bmin.z
	);
	m_VoxelGenShader.Use();
	m_VoxelGenShader.setMatrix4fv(uProjectionMatrixStr.c_str(), Proj);

	glm::vec3 Resolution = glm::vec3(m_NodeDepth, m_NodeDepth, m_NodeDepth);
	m_VoxelGenShader.setVec3f(uResolutionStr.c_str(), Resolution);

	int StartIndex = 0;
	m_NodeCreationShader.Use();
	m_NodeCreationShader.setInt(uStartIndexStr.c_str(), StartIndex);
	m_MipMapShader.Use();
	m_MipMapShader.setInt(uStartIndexStr.c_str(), StartIndex);

	Update();
}

void SparseVoxelOctree::Bind() const
{
	m_NodePool.BindBase(2);
	m_NodeColor.BindBase(3);
}

void SparseVoxelOctree::Update()
{
	double t1 = 0.f;
	GLuint voxelCount, nodeCount;

	////////////////////////////////////////////////////////////////////////////////////
	// clear and bind buffers, textures, atomic counters, etc.
	//bind Buffers
	m_VoxelFragmentList.BindBase(0);
	m_VoxelFragmentColor.BindBase(1);
	m_NodePool.BindBase(2);
	m_NodeColor.BindBase(3);
	m_VoxelCounter.BindBase(0);
	m_NodeCounter.BindBase(1);

	glNamedBufferSubData(m_VoxelCounter.GetHandle(), 0, 4, &voxelCount);
	glNamedBufferSubData(m_NodeCounter.GetHandle(), 0, 4, &nodeCount);

	glm::vec4 zero_vec = glm::vec4(0.f);
	const GLint clear_val = -1;
	glClearNamedBufferSubData(m_VoxelFragmentList.GetHandle(), GL_RGBA32F, 0, m_VoxelFragmentListBufferSize, GL_RGBA, GL_FLOAT, glm::value_ptr(zero_vec));
	glClearNamedBufferSubData(m_VoxelFragmentColor.GetHandle(), GL_RGBA32F, 0, m_VoxelFragmentColorBufferSize, GL_RGBA, GL_FLOAT, glm::value_ptr(zero_vec));
	glClearNamedBufferSubData(m_NodeColor.GetHandle(), GL_RGBA32F, 0, m_NodeColorBufferSize, GL_RGBA, GL_FLOAT, glm::value_ptr(zero_vec));
	glClearNamedBufferSubData(m_NodePool.GetHandle(), GL_R32I, 0, m_NodePoolBufferSize, GL_RED_INTEGER, GL_INT, &clear_val);

	GLuint clear_val_atomic = 0u;
	glClearNamedBufferSubData(m_VoxelCounter.GetHandle(), GL_R32UI, 0, 4, GL_RED_INTEGER, GL_UNSIGNED_INT, &clear_val_atomic);
	clear_val_atomic = 1u;
	glClearNamedBufferSubData(m_NodeCounter.GetHandle(), GL_R32UI, 0, 4, GL_RED_INTEGER, GL_UNSIGNED_INT, &clear_val_atomic);

	const GLint root_val = 8;
	m_NodePool.SetContentSubData(root_val, 0);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	////////////////////////////////////////////////////////////////////////////////////
	// Voxelization into uniform grid, i.e. creation of voxel fragment list

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CONSERVATIVE_RASTERIZATION_INTEL);
	const float Nf = static_cast<float>(m_NodeDepth);
	glViewportIndexedf(1, 0.0f, 0.0f, Nf, Nf);
	glViewportIndexedf(2, 0.0f, 0.0f, Nf, Nf);
	glViewportIndexedf(3, 0.0f, 0.0f, Nf, Nf);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_VoxelGenShader.Use();
	std::for_each(m_Scene.begin(), m_Scene.end(), [&](auto& mesh)
	{
		m_VoxelGenShader.setMatrix4fv(uModelMatrixStr.c_str(), mesh.GetModelTransform());
		lesson_3n1::CDrawFileMeshData::MeshDraw(m_VoxelGenShader, mesh);
	});

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	glDisable(GL_CONSERVATIVE_RASTERIZATION_INTEL);

	glGetNamedBufferSubData(m_VoxelCounter.GetHandle(), 0, 4, &voxelCount);

	///////////////////////////////////////////////////////////////////////////////////
	// Octree generation
	std::vector<int> levelStartIndices = {};
	levelStartIndices.push_back(8);

	for (int i = 1; i < m_Depth; ++i)
	{
		m_FlagShader.Use();
		glDispatchCompute(static_cast<GLuint>(glm::ceil(voxelCount / 64.f)), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glGetNamedBufferSubData(m_NodeCounter.GetHandle(), 0, 4, &nodeCount);
		levelStartIndices.push_back(static_cast<int>(nodeCount + 1) * 8);
		m_NodeCreationShader.Use();
		m_NodeCreationShader.setInt(uStartIndexStr.c_str(), levelStartIndices[i - 1]);
		glDispatchCompute(static_cast<GLuint>(glm::pow(8, i - 1)), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	}

	glGetNamedBufferSubData(m_NodeCounter.GetHandle(), 0, 4, &nodeCount);
	levelStartIndices.push_back(static_cast<int>(nodeCount + 1) * 8);

	///////////////////////////////////////////////////////////////////////////////////
	// Initialization of leaf node data

	m_LeafInitShader.Use();
	glDispatchCompute(static_cast<GLuint>(glm::ceil(voxelCount / 64.f)), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	///////////////////////////////////////////////////////////////////////////////////
	// Mipmap values

	m_MipMapShader.Use();
	for (int64_t i = m_Depth - 2; i >= 0; --i)
	{
		m_MipMapShader.setInt(uStartIndexStr.c_str(), levelStartIndices[i]);
		glDispatchCompute((levelStartIndices[i + 1] - levelStartIndices[i]) / 8, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	m_MipMapShader.setInt(uStartIndexStr.c_str(), 0);
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

const glm::vec3& SparseVoxelOctree::GetBMin() const
{
	return m_Bmin;
}

const glm::vec3& SparseVoxelOctree::GetBMax() const
{
	return m_Bmax;
}
