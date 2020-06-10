#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "../1n5_shaders/Shader.h"

namespace lesson_3n1
{
    struct SVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    enum ETextureType
    {
        DIFFUSE = 0,
        SPECULAR = 1,
    };

    struct CAssimpHelpData
    {
        static CAssimpHelpData& Get()
        {
            static CAssimpHelpData data;
            return data;
        }

        const std::vector<std::string> mTextureType
        {
            "texture_diffuse_",
            "texture_specular_",
        };

    private:

        CAssimpHelpData() = default;
        CAssimpHelpData(const CAssimpHelpData&) = delete;
        void operator=(const CAssimpHelpData&) = delete;
    };

    struct STexture {

        unsigned int id;
        std::string typeName;
        ETextureType type;
        std::string path;

        void SetType(ETextureType t)
        {
            type = t;
            typeName = CAssimpHelpData::Get().mTextureType[t];
        }
    };

    struct SMesh {

    public:
        /*  Functions  */
        SMesh(std::vector<SVertex>& vertices, std::vector<unsigned int>& indices, std::vector<STexture>& textures);

        const std::vector<SVertex>& GetVertices() const { return mVertices; };
        const std::vector<unsigned int>& GetIndices() const { return mIndices; };
        const std::vector<STexture>& GetTextures() const { return mTextures; }

        unsigned int GetVAO() const { return VAO; };
        unsigned int GetVBO() const { return VBO; };
        unsigned int GetEBO() const { return EBO; };

        void SetVAO(unsigned int& vao) { VAO = vao; };
        void SetEBO(unsigned int& ebo) { EBO = ebo; };
        void SetVBO(unsigned int& vbo) { VBO = vbo; };

    private:
        /*  Mesh Data  */
        std::vector<SVertex> mVertices = {};
        std::vector<unsigned int> mIndices = {};
        std::vector<STexture> mTextures = {};
        /*  Render data  */
        unsigned int VAO, VBO, EBO;
        /*  Functions    */
    };

    struct SFileMeshData
    {
        std::vector<SMesh> meshes;
    };

    class CDrawFileMeshData
    {
    public:

        static void Init(SFileMeshData& fileMesh);
        static void Draw(lesson_1n5::CShader& shader, SFileMeshData& fileMesh);
        static void DrawInstanced(lesson_1n5::CShader& shader, SFileMeshData& fileMesh, std::size_t amountInstances);
        static void DeleteAfterLoop(SFileMeshData& fileMesh);

    protected:

        static void MeshInit(SMesh& mesh);
        static void MeshDraw(lesson_1n5::CShader& shader, const SMesh& mesh);
        static void MeshDrawInstanced(lesson_1n5::CShader& shader, const SMesh& mesh, std::size_t amountInstances);
        static void BindTextures(lesson_1n5::CShader& shader, const SMesh& mesh);
    };

    class CLoadAssimpFile
    {
    public:

        static void Load(std::string path, SFileMeshData& output);

    protected:

        struct SLoadAssimpFileData
        {
            SFileMeshData* fileMeshData = nullptr;
            std::string directory;
            std::unordered_map<std::size_t, STexture> texturesLoaded;
        };

        static void ProcessNode(aiNode* node, const aiScene* scene, SLoadAssimpFileData&);

        static void ProcessMesh(aiMesh* mesh, const aiScene* scene, SLoadAssimpFileData&);

        static void LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::vector<STexture>& textures, SLoadAssimpFileData&);
    };
}