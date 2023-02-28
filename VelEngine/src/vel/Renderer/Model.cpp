#include "velpch.h"

#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/texture.h>

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include "Renderer.h"

namespace vel
{
    Model::Model(std::string source, bool useTextures)
        :m_UseFBXTextures(useTextures)
	{
        m_Meshes = std::vector<MeshData>();
		m_Importer = std::make_unique<Assimp::Importer>();
        const aiScene* scene = m_Importer->ReadFile(source, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
           VEL_CORE_ERROR("ERROR::ASSIMP:: {0}", m_Importer->GetErrorString());
            return;
        }
        m_Path = source.substr(0, source.find_last_of('/'));

        ProcessNode(scene->mRootNode, scene);
	}

    MeshData Model::GetMeshData()
    {
        MeshData wholeMeshData;
        for (int i = 0; i < m_Meshes.size(); i++)
        {
            wholeMeshData.m_Vertices.insert(
                wholeMeshData.m_Vertices.end(), 
                m_Meshes[i].m_Vertices.begin(), 
                m_Meshes[i].m_Vertices.end()
            );

            wholeMeshData.m_Indices.insert(
                wholeMeshData.m_Indices.end(),
                m_Meshes[i].m_Indices.begin(),
                m_Meshes[i].m_Indices.end()
            );

            wholeMeshData.m_Textures.insert(
                wholeMeshData.m_Textures.end(),
                m_Meshes[i].m_Textures.begin(),
                m_Meshes[i].m_Textures.end()
            );
        }
        return wholeMeshData;
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(mesh, scene));
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    MeshData Model::ProcessMesh(aiMesh* aimesh, const aiScene* scene)
    {
        MeshData newMeshData;

        // Get all the vertex data
        for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
        {
            Vertices vertex;
            vertex.x = aimesh->mVertices[i].x;
            vertex.y = aimesh->mVertices[i].y;
            vertex.z = aimesh->mVertices[i].z;
            
            if (aimesh->HasNormals())
            {
                vertex.nx = aimesh->mNormals[i].x;
                vertex.ny = aimesh->mNormals[i].y;
                vertex.nz = aimesh->mNormals[i].z;
            }

            if (aimesh->mTextureCoords[0])
            {
                vertex.u0 = aimesh->mTextureCoords[0][i].x;
                vertex.v0 = aimesh->mTextureCoords[0][i].y;

                if (aimesh->mTangents)
                {
                    vertex.tx = aimesh->mTangents[i].x;
                    vertex.ty = aimesh->mTangents[i].y;
                    vertex.tz = aimesh->mTangents[i].z;
                }

                if (aimesh->mBitangents)
                {
                    vertex.bx = aimesh->mBitangents[i].x;
                    vertex.by = aimesh->mBitangents[i].y;
                    vertex.bz = aimesh->mBitangents[i].z;
                }
            }
            else {
                vertex.u0 = 0.f;
                vertex.v0 = 0.f;
            }

            newMeshData.m_Vertices.push_back(vertex);
        }

        // Get all the indices
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
        {
            aiFace face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                newMeshData.m_Indices.push_back(face.mIndices[j]);
        }


        if (m_UseFBXTextures)
        {
            // Process the materials
            aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
            std::vector<Ref<Texture2D>> diffuseTextures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            newMeshData.m_Textures.insert(newMeshData.m_Textures.end(), diffuseTextures.begin(), diffuseTextures.end());
        }
        newMeshData.m_VertexArray = VertexArray::Create();
        vel::Ref<vel::VertexBuffer> vertexBuffer;
        vertexBuffer = vel::VertexBuffer::Create(
            &newMeshData.m_Vertices[0],
            sizeof(vel::Vertices) * newMeshData.m_Vertices.size()
        );
        vertexBuffer->SetLayout({
                { vel::ShaderDataType::Float4, "vColour"},
                { vel::ShaderDataType::Float4, "vPosition"},
                { vel::ShaderDataType::Float4, "vNormal"},
                { vel::ShaderDataType::Float4, "vUVx2"},
                { vel::ShaderDataType::Float4, "vTangent"},
                { vel::ShaderDataType::Float4, "vBiNormal"},
                { vel::ShaderDataType::Float4, "vBoneID"},
                { vel::ShaderDataType::Float4, "vBoneWeight"}
            });
         newMeshData.m_VertexArray->AddVertexBuffer(vertexBuffer);
         newMeshData.m_VertexArray->SetIndexBuffer(
            IndexBuffer::Create(
                &newMeshData.m_Indices[0],
                newMeshData.m_Indices.size()
            )
        );
        
        return newMeshData;
        
        /*
        std::vector<Ref<Texture2D>> speculatTextures = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        newMeshData.m_Textures.insert(newMeshData.m_Textures.end(), speculatTextures.begin(), speculatTextures.end());

        
        std::vector<Ref<Texture2D>> normalTextures = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        newMeshData.m_Textures.insert(newMeshData.m_Textures.end(), normalTextures.begin(), normalTextures.end());

        
        std::vector<Ref<Texture2D>> heightTextures = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        newMeshData.m_Textures.insert(newMeshData.m_Textures.end(), heightTextures.begin(), heightTextures.end());

        return newMeshData;
        */
    }

    std::vector<Ref<Texture2D>>  Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typesname)
    {
        std::vector<Ref<Texture2D>> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;

            for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
            {
                if (std::strcmp(m_TexturesLoaded[j]->GetPath().c_str(), str.C_Str()) == 0)
                {
                    // texture with same path already loaded
                    textures.push_back(m_TexturesLoaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                // if texture wasn't in the cache then load it
                std::string fileName = str.C_Str();
                fileName = fileName.substr(fileName.find_last_of('/') + 1, fileName.size());
                fileName = fileName.substr(fileName.find_last_of("\\") + 1, fileName.size());
                size_t size = fileName.find('.');
                if (size > fileName.size())
                    continue;
                Ref<Texture2D > texture2D = Texture2D::Create(this->m_Path + '/' + fileName);
                textures.push_back(texture2D);
                m_TexturesLoaded.push_back(texture2D);
            }
        }
        return textures;
    }
    void Model::DrawMesh(Ref<Shader> shader, const glm::mat4& transform)
    {
        if (m_Meshes.empty())
            return;
        for (MeshData meshData : m_Meshes)
        {
           /* for (Ref<Texture2D> texture : meshData.m_Textures)
            {
                texture->Bind();
            }*/
            if(meshData.m_Textures.size() > 0)
                meshData.m_Textures[0]->Bind();
            Renderer::Submit(shader, meshData.m_VertexArray, transform);
        }
    }
    void Model::DrawMesh(Ref<Shader> shader, const Ref<Material> material, const glm::mat4& transform)
    {
        if (m_Meshes.empty())
            return;
        for (MeshData meshData : m_Meshes)
        {
            if (meshData.m_VertexArray == nullptr)
                continue;

            std::vector<Ref<Texture2D>> textures;
            shader->Bind();

            if (m_UseFBXTextures)
            {
                shader->SetBool("useTextureNormal", false);
                shader->SetBool("useTextureSpecular", false);
                shader->SetFloat4("RGBA", material->Diffuse);
                shader->SetFloat4("SPEC", material->Specular);
                shader->SetBool("useTextureDiffuse", true);
                Renderer::Submit(shader, meshData.m_VertexArray, meshData.m_Textures, transform);
            }
            else {
                bool useDifMap = false;
                bool useSpeMap = false;
                bool useNorMap = false;

                if (material->DiffuseTexture != nullptr && material->DiffuseTexture->IsLoaded())
                {
                    useDifMap = true;
                }
                shader->SetFloat4("RGBA", material->Diffuse);

                if (material->SpecularTexture != nullptr && material->SpecularTexture->IsLoaded())
                {
                    useSpeMap = true;
                }
                shader->SetFloat4("SPEC", material->Specular);

                if (material->NormalTexture != nullptr && material->NormalTexture->IsLoaded())
                {
                    useNorMap = true;
                }
                shader->SetBool("useTextureNormal", useNorMap);
                shader->SetBool("useTextureSpecular", useSpeMap);
                shader->SetBool("useTextureDiffuse", useDifMap);

                if(useDifMap)
                    textures.push_back(material->DiffuseTexture);
                if(useNorMap)
                    textures.push_back(material->NormalTexture);
                if(useSpeMap)
                    textures.push_back(material->SpecularTexture);

                Renderer::Submit(shader, meshData.m_VertexArray, textures, transform);
            }
        }
    }
}