#include "AssimpData.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <algorithm>
#include <sstream>
#include <iostream>

#include "LoadTexture.h"

using namespace lesson_3n1;

SMesh::SMesh(std::vector<SVertex>& vertices, std::vector<unsigned int>& indices, std::vector<STexture>& textures)
{
    mVertices.resize(vertices.size());
    std::copy(vertices.begin(), vertices.end(), mVertices.begin());
    mIndices.resize(indices.size());
    std::copy(indices.begin(), indices.end(), mIndices.begin());
    mTextures.resize(textures.size());
    std::copy(textures.begin(), textures.end(), mTextures.begin());
}

void CDrawFileMeshData::MeshInit(SMesh& mesh)
{
    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    mesh.SetVAO(VAO);
    mesh.SetVBO(VBO);
    mesh.SetEBO(EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.GetVertices().size() * sizeof(SVertex), &mesh.GetVertices()[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndices().size() * sizeof(unsigned int),
        &mesh.GetIndices()[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, TexCoords));

    glBindVertexArray(0);
}

void CDrawFileMeshData::MeshDraw(lesson_1n5::CShader& shader, const SMesh& mesh)
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;

    std::size_t texturesSize = mesh.GetTextures().size();
    const std::vector<STexture>& textures = mesh.GetTextures();

    for (std::size_t i = 0; i < texturesSize; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::stringstream ss;
        std::string number;
        ETextureType type = textures[i].type;
        const std::string& name = textures[i].typeName;
        if (type == ETextureType::DIFFUSE)
            ss << diffuseNr++; 
        else if (type == ETextureType::SPECULAR)
            ss << specularNr++;
        number = ss.str();

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);

    unsigned int VAO = mesh.GetVAO();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CDrawFileMeshData::Init(SFileMeshData& fileMesh)
{
    std::size_t meshesSize = fileMesh.meshes.size();
    std::vector<SMesh>& meshes = fileMesh.meshes;

    for (std::size_t i = 0; i < meshesSize; i++)
        MeshInit(meshes[i]);
}

void CDrawFileMeshData::Draw(lesson_1n5::CShader& shader, SFileMeshData& fileMesh)
{
    std::size_t meshesSize = fileMesh.meshes.size();
    const std::vector<SMesh>& meshes = fileMesh.meshes;

    for (std::size_t i = 0; i < meshesSize; i++)
        MeshDraw(shader, meshes[i]);
}

void CLoadAssimpFile::Load(std::string path, SFileMeshData& output)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    SLoadAssimpFileData data;
    data.fileMeshData = &output;
    data.directory = path.substr(0, path.find_last_of('/'));

    CLoadAssimpFile::ProcessNode(scene->mRootNode, scene, data);
}

void CLoadAssimpFile::ProcessNode(aiNode* node, const aiScene* scene, SLoadAssimpFileData& data)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        CLoadAssimpFile::ProcessMesh(mesh, scene, data);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        CLoadAssimpFile::ProcessNode(node->mChildren[i], scene, data);
    }
}

void CLoadAssimpFile::ProcessMesh(aiMesh* mesh, const aiScene* scene, SLoadAssimpFileData& data)
{
    std::vector<SVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<STexture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        SVertex vertex;

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

        vertices.emplace_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<STexture> diffuseMaps = {};
        CLoadAssimpFile::LoadMaterialTextures(material,
            aiTextureType_DIFFUSE, diffuseMaps, data);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<STexture> specularMaps = {};
        CLoadAssimpFile::LoadMaterialTextures(material,
            aiTextureType_SPECULAR, specularMaps, data);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    SMesh contentMesh(vertices, indices, textures);
    data.fileMeshData->meshes.emplace_back(contentMesh);
}

void CLoadAssimpFile::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::vector<STexture>& textures, SLoadAssimpFileData& data)
{
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        STexture texture;
        std::size_t hash = std::hash<std::string>{}(str.C_Str());
        auto it = data.texturesLoaded.find(hash);
        if (it == data.texturesLoaded.end())
        {
            texture.path = data.directory + "/" + std::string(str.C_Str());
            texture.id = CLoadTexture::TextureFromFile(str.C_Str(), data.directory);

            switch (type)
            {
            case aiTextureType_DIFFUSE:
                texture.SetType(ETextureType::DIFFUSE);
                break;
            case aiTextureType_SPECULAR:
                texture.SetType(ETextureType::SPECULAR);
                break;
            default:
                break;
            }

            textures.push_back(texture);
            data.texturesLoaded.emplace(std::make_pair(hash, texture));
        }
        else
        {
            textures.push_back((*it).second);
        }
    }
}

void CDrawFileMeshData::DeleteAfterLoop(SFileMeshData& fileMesh)
{
    std::size_t meshesSize = fileMesh.meshes.size();
    const std::vector<SMesh>& meshes = fileMesh.meshes;
    unsigned int VAO, VBO, EBO;

    for (std::size_t i = 0; i < meshesSize; i++)
    {
        VAO = meshes[i].GetVAO();
        glDeleteVertexArrays(1, &VAO);
        VBO = meshes[i].GetVBO();
        glDeleteBuffers(1, &VBO);
        EBO = meshes[i].GetEBO();
        glDeleteBuffers(1, &EBO);
    }
}