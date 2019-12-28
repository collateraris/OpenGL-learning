#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Functions.h"

#include <list>
#include <vector>
#include <string>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#ifndef __MODEL_ASSIMP__
#define __MODEL_ASSIMP__

class Model
{
public:
	 
	Model(const char *path)
	{
		loadModel(path);
	}

	void Draw(class Shader* shader);

private:
	std::list<Texture> textures_loaded;
	std::vector<Mesh*> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(class aiNode* node, const class aiScene* scene);
	Mesh* processMesh(class aiMesh* mesh, const class aiScene* scene);
	void loadMaterialTextures(class aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<Texture>& outTexture);

};

void Model::Draw(Shader* shader)
{
	for (Mesh* mesh : meshes)
	{
		mesh->Draw(shader);
	}
}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);

	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0]) 
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	int numIndices = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		numIndices += mesh->mFaces[i].mNumIndices;
	}
	indices.reserve(numIndices);

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
		
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps;
		loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", diffuseMaps);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps;
		loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", specularMaps);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps;
		loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", normalMaps);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		
		std::vector<Texture> heightMaps;
		loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", heightMaps);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		std::vector<Texture> unknowMaps;
		loadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_unknow", unknowMaps);
		textures.insert(textures.end(), unknowMaps.begin(), unknowMaps.end());

	}

	return new Mesh(vertices, indices, textures);
}

void Model::loadMaterialTextures(class aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<Texture>& outTexture)
{
	outTexture.reserve(mat->GetTextureCount(type));
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (Texture& texture : textures_loaded)
		{
			if (std::strcmp(texture.path.C_Str(), str.C_Str()) == 0)
			{
				outTexture.push_back(texture);
				skip = true;
				break;
			}
		}

		if (skip)
		{
			continue;
		}

		Texture texture;
		texture.id = Functions::TextureFromFile(str.C_Str(), directory);
		texture.type = typeName;
		texture.path = str;
		textures_loaded.push_back(texture);
		outTexture.push_back(texture);
	}
}

#endif
