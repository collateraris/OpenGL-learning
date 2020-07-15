#include "AssimpData.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <assimp/pbrmaterial.h>

#include <algorithm>
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

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, Tangent));

    glBindVertexArray(0);
}

void CDrawFileMeshData::BindTextures(lesson_1n5::CShader& shader, const SMesh& mesh)
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int ambientNr = 0;

    std::size_t texturesSize = mesh.GetTextures().size();
    const std::vector<STexture>& textures = mesh.GetTextures();

    for (std::size_t i = 0; i < texturesSize; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        ETextureType type = textures[i].type;
        const std::string& name = textures[i].typeName;
        switch (type)
        {
        case lesson_3n1::DIFFUSE:
            number = std::to_string(diffuseNr++);
            break;
        case lesson_3n1::SPECULAR:
            number = std::to_string(specularNr++);
            break;
        case lesson_3n1::NORMAL:
            number = std::to_string(normalNr++);
            break;
        case lesson_3n1::AMBIENT:
            number = std::to_string(ambientNr++);
            break;
        default:
            continue;
        }

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);
}

void CDrawFileMeshData::MeshDraw(lesson_1n5::CShader& shader, const SMesh& mesh)
{
    CDrawFileMeshData::BindTextures(shader, mesh);

    unsigned int VAO = mesh.GetVAO();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CDrawFileMeshData::MeshDrawInstanced(lesson_1n5::CShader& shader, const SMesh& mesh, std::size_t amountInstances)
{
    CDrawFileMeshData::BindTextures(shader, mesh);

    unsigned int VAO = mesh.GetVAO();
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0, amountInstances );
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
        CDrawFileMeshData::MeshDraw(shader, meshes[i]);
}

void CDrawFileMeshData::DrawInstanced(lesson_1n5::CShader& shader, SFileMeshData& fileMesh, std::size_t amountInstances)
{
    std::size_t meshesSize = fileMesh.meshes.size();
    const std::vector<SMesh>& meshes = fileMesh.meshes;

    for (std::size_t i = 0; i < meshesSize; i++)
        CDrawFileMeshData::MeshDrawInstanced(shader, meshes[i], amountInstances);
}

void CLoadAssimpFile::Load(std::string path, SFileMeshData& output)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

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

        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;

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
        CLoadAssimpFile::LoadMaterialTextures(material,
            aiTextureType_DIFFUSE, textures, data);

        CLoadAssimpFile::LoadMaterialTextures(material,
            aiTextureType_SPECULAR, textures, data);

        CLoadAssimpFile::LoadMaterialTextures(material,
            aiTextureType_HEIGHT, textures, data);
        CLoadAssimpFile::LoadMaterialTextures(material,
            aiTextureType_NORMALS , textures, data);

        CLoadAssimpFile::LoadMaterialTextures(material, aiTextureType_AMBIENT, textures, data);
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

            switch (type)
            {
            case aiTextureType_DIFFUSE:
                texture.id = CLoadTexture::GammaTextureFromFile(str.C_Str(), data.directory);
                texture.SetType(ETextureType::DIFFUSE);
                break;
            case aiTextureType_SPECULAR:
                texture.id = CLoadTexture::TextureFromFile(str.C_Str(), data.directory);
                texture.SetType(ETextureType::SPECULAR);
                break;
            case aiTextureType_HEIGHT:
            case aiTextureType_NORMALS:
                texture.id = CLoadTexture::TextureFromFile(str.C_Str(), data.directory);
                texture.SetType(ETextureType::NORMAL);
                break;
            case aiTextureType_AMBIENT:
                texture.id = CLoadTexture::TextureFromFile(str.C_Str(), data.directory);
                texture.SetType(ETextureType::AMBIENT);
                break;
            default:
                continue;
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