#include "velpch.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/texture.h>

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include "Renderer.h"
#include <vel/Scene/EntityManager.h>
#include <vel/Scene/Scene.h>
#include "vel/Scene/Entity.h"
#include "vel/Scene/Component.h"
#include "vel/Scene/SaveSystem.h"
#include "vel/Scene/MeshRenderer.h"
#include "vel/Core/GUID.h"
#include "vel/Scene/MaterialSystem.h"
#include "vel/Math/AssimpGLMHelpers.h"
#include "Animation/Animation.h"

// controls acces to the vector of m_Meshes
CRITICAL_SECTION ModelLoader_Lock;

namespace vel
{
    Model::Model(ModelLoadData data)
        :modelLoadData(data), m_GameScene(data.gameScene)
        ,m_UseFBXTextures(data.useTextures), m_LoadAsync(data.loadAsync), m_MaterialPath(data.materialPath)
    {
        m_InitCriticalSections();
        m_Meshes = std::vector<Ref<MeshData>>();
        m_Importer = CreateScope<Assimp::Importer>();
        const aiScene* scene = m_Importer->ReadFile(data.source, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            VEL_CORE_ERROR("ERROR::ASSIMP:: {0}", m_Importer->GetErrorString());
            return;
        }
        m_Path = modelLoadData.source.substr(0, modelLoadData.source.find_last_of('/'));
        m_Name = modelLoadData.source.substr(modelLoadData.source.find_last_of('/') + 1, modelLoadData.source.find_first_of('.'));

        std::string path = m_Path + "/" + m_Name + ".vasset";

        if (m_GameScene)
        {
            bool entitycreated = false;
            if (modelLoadData.assetData.size() > 0)
            {
                for (auto const& [key, val] : modelLoadData.assetData)
                {
                    if (val.Name == m_Name)
                    {
                        m_ModelPrefab = &m_GameScene->CreateAssetEntityWithID(key, *modelLoadData.parentEntity,m_Name, false, IsAsset);
                        entitycreated = true;
                        break;
                    }
                }
            }
            if(!entitycreated)
                m_ModelPrefab = &m_GameScene->CreateChildEntity(*modelLoadData.parentEntity,m_Name, IsAsset);
            m_AssetHandle.push_back({ m_ModelPrefab->Name(),m_ModelPrefab->GetGUID() , m_ModelPrefab->GetAssetID(), ""});
        }
        m_MaterialIns = modelLoadData.material;
        ProcessNode(scene->mRootNode, scene, m_ModelPrefab);

        VEL_CORE_INFO("{0} Model loaded", m_Name);
    }
    Model::Model(std::string source, bool useTextures, bool loadAsync)
        :m_UseFBXTextures(useTextures), m_LoadAsync(loadAsync)
	{
        m_InitCriticalSections();
        m_Meshes = std::vector<Ref<MeshData>>();
		m_Importer = std::make_unique<Assimp::Importer>();
        const aiScene* scene = m_Importer->ReadFile(source, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
           VEL_CORE_ERROR("ERROR::ASSIMP:: {0}", m_Importer->GetErrorString());
            return;
        }
        m_Path = source.substr(0, source.find_last_of('/')); 
        m_Name = source.substr(source.find_last_of('/') + 1, source.size());


        ProcessNode(scene->mRootNode, scene, nullptr);

        VEL_CORE_TRACE("{0} Model loaded", m_Name);
	}

    Model::Model(std::string source, bool useTextures, bool loadAsync, Scene* gameScene)
        :m_UseFBXTextures(useTextures), m_LoadAsync(loadAsync),m_GameScene(gameScene)
    {
        m_InitCriticalSections();
        m_Meshes = std::vector<Ref<MeshData>>();
        m_Importer = std::make_unique<Assimp::Importer>();
        const aiScene* scene = m_Importer->ReadFile(source, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            VEL_CORE_ERROR("ERROR::ASSIMP:: {0}", m_Importer->GetErrorString());
            return;
        }
        m_Path = source.substr(0, source.find_last_of('/'));
        m_Name = source.substr(source.find_last_of('/') + 1, source.find('.'));
        //Entity* entity = nullptr;
        if (m_GameScene)
        {
            m_ModelPrefab = &m_GameScene->CreateEntity(m_Name, IsAsset);
            m_ModelPrefab->SetScene(m_GameScene);
           // m_ModelPrefab = entity;
            m_AssetHandle.push_back({ m_ModelPrefab->Name(),m_ModelPrefab->GetGUID(),m_ModelPrefab->GetAssetID(), ""});
        }
        ProcessNode(scene->mRootNode, scene, m_ModelPrefab);

        VEL_CORE_TRACE("{0} Model loaded", m_Name);
    }

    Model::~Model() { m_DeleteCriticalSections(); }

    MeshData Model::GetMeshData()
    {
        MeshData wholeMeshData;
        for (int i = 0; i < m_Meshes.size(); i++)
        {
            wholeMeshData.m_Vertices.insert(
                wholeMeshData.m_Vertices.end(), 
                m_Meshes[i]->m_Vertices.begin(), 
                m_Meshes[i]->m_Vertices.end()
            );

            wholeMeshData.m_Indices.insert(
                wholeMeshData.m_Indices.end(),
                m_Meshes[i]->m_Indices.begin(),
                m_Meshes[i]->m_Indices.end()
            );

            wholeMeshData.m_Textures.insert(
                wholeMeshData.m_Textures.end(),
                m_Meshes[i]->m_Textures.begin(),
                m_Meshes[i]->m_Textures.end()
            );
            wholeMeshData.m_VertexArray = m_Meshes[i]->m_VertexArray;
        }
        return wholeMeshData;
    }

    struct MeshLoadingInfo
    {
        aiMesh* aimesh;
        const aiScene* scene;
        bool m_UseFBXTextures;
        std::vector<MeshData>* m_Meshes;
    };

    void Model::m_InitCriticalSections(void)
    {
        InitializeCriticalSection(&ModelLoader_Lock);
    }
    void Model::m_DeleteCriticalSections(void)
    {
        DeleteCriticalSection(&ModelLoader_Lock);
    }

    void Model::SetVertexBoneDataToDefault(Vertices* vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex->vBoneID[i] = -1;
            vertex->vBoneWeight[i] = 0.0f;
        }
    }

    void Model::SetVertexBoneData(Vertices* vertex, int boneID, float weight)
    {
        if (boneID >= 100)
            VEL_CORE_ERROR("Invalid bone ID for {0}, {1}", m_Name, boneID);
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if (vertex->vBoneID[i] == -1)
            {
                vertex->vBoneID[i] = boneID;
                vertex->vBoneWeight[i] = weight;
                return;
            }
        }
        VEL_CORE_ERROR("Bone not assigned for {0},{1}", m_Name, boneID);
    }

    void Model::ExtractBoneWeightForVertices(Ref<MeshData> meshData, aiMesh* mesh, const aiScene* scene)
    {
        meshData->m_UseBones = true;
        std::vector<Vertices>* vertices = &meshData->m_Vertices;
        m_BoneCounter = 0;
        meshData->m_BoneInfoMap = new std::unordered_map<std::string, BoneInfo>();
        meshData->m_BoneInfoMap->reserve(mesh->mNumBones);
        VEL_CORE_INFO("{0} has {1} bones", mesh->mName.data, mesh->mNumBones);
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (meshData->m_BoneInfoMap->find(boneName) == meshData->m_BoneInfoMap->end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = meshData->m_BoneCounter;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                    mesh->mBones[boneIndex]->mOffsetMatrix);
                meshData->m_BoneInfoMap->emplace(boneName,newBoneInfo);
                boneID = meshData->m_BoneCounter;
                meshData->m_BoneCounter++;
            }
            else
            {
                boneID = meshData->m_BoneInfoMap->at(boneName).id;
            }
            VEL_CORE_ASSERT(boneID != -1, "Bone is invalid");
            VEL_CORE_WARN("{0} Adding {1}, index: {2}",m_Name,boneName, boneID);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                VEL_CORE_ASSERT(vertexId <= vertices->size(), "VertexId is invalid");
                SetVertexBoneData(&(vertices->at(vertexId)), boneID, weight);
            }
        }
    }

    //DWORD WINAPI ProcessMesh1(LPVOID pParameters)
    Ref<MeshData> Model::ProcessMesh(aiMesh* aimesh, const aiScene* scene)
    {
        /*MeshLoadingInfo* fileInfo = (MeshLoadingInfo*)pParameters;
        aiMesh* aimesh = fileInfo->aimesh;
        const aiScene* scene = fileInfo->scene;

        bool m_UseFBXTextures = fileInfo->m_UseFBXTextures;
        std::vector<MeshData>* m_Meshes = fileInfo->m_Meshes;*/

        Ref<MeshData> newMeshData = CreateRef<MeshData>();
        int numVer = 0;
        // Get all the vertex data
        VEL_CORE_TRACE("{0}: Total vertices: {1}", m_Name, aimesh->mNumVertices);
        for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
        {
            Vertices vertex;
            SetVertexBoneDataToDefault(&vertex);
            numVer++;
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

            newMeshData->m_Vertices.push_back(vertex);
        }
        VEL_CORE_TRACE("{0}: Total vertices transformed: {1}", m_Name, numVer);

        // Get all the indices
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
        {
            aiFace face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                newMeshData->m_Indices.push_back(face.mIndices[j]);
        }
        if (aimesh->HasBones())
        {
            ExtractBoneWeightForVertices(newMeshData, aimesh, scene);
        }

        if (m_UseFBXTextures)
        {
            // Process the materials
            aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
            std::vector<Ref<Texture2D>> diffuseTextures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            newMeshData->m_Textures.insert(newMeshData->m_Textures.end(), diffuseTextures.begin(), diffuseTextures.end());
        }
        newMeshData->m_Loaded = true;
        /*EnterCriticalSection(&ModelLoader_Lock);

        m_Meshes->push_back(newMeshData);

        LeaveCriticalSection(&ModelLoader_Lock);*/

        if (!m_LoadAsync)
        {
            newMeshData->m_VertexArray = VertexArray::Create();
            vel::Ref<vel::VertexBuffer> vertexBuffer;
            vertexBuffer = vel::VertexBuffer::Create(
                &newMeshData->m_Vertices[0],
                sizeof(vel::Vertices) * newMeshData->m_Vertices.size()
            );
            vertexBuffer->SetLayout({
                    { vel::ShaderDataType::Float4, "vColour"},
                    { vel::ShaderDataType::Float4, "vPosition"},
                    { vel::ShaderDataType::Float4, "vNormal"},
                    { vel::ShaderDataType::Float4, "vUVx2"},
                    { vel::ShaderDataType::Float4, "vTangent"},
                    { vel::ShaderDataType::Float4, "vBiNormal"},
                    { vel::ShaderDataType::Int4, "vBoneID"},
                    { vel::ShaderDataType::Float4, "vBoneWeight"}
                });
            newMeshData->m_VertexArray->AddVertexBuffer(vertexBuffer);
            Ref<IndexBuffer> ib = IndexBuffer::Create(
                &newMeshData->m_Indices[0],
                newMeshData->m_Indices.size()
            );
            newMeshData->m_VertexArray->SetIndexBuffer(ib);
        }

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

    void Model::ProcessNode(aiNode* node, const aiScene* scene, Entity* parentEntity)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            
            //VEL_CORE_INFO("Mesh: {0}, Node: {1}", mesh->mName.C_Str(), node->mName.C_Str());

            /*MeshLoadingInfo* fileInfo = new MeshLoadingInfo();
            fileInfo->aimesh = mesh;
            fileInfo->scene = scene;
            fileInfo->m_Meshes = &m_Meshes;
            fileInfo->m_UseFBXTextures = m_UseFBXTextures;

            LPDWORD lpThreadId = 0;

            HANDLE hSceneLoader =
                CreateThread(
                    NULL,
                    0,
                    ProcessMesh1,
                    (void*)fileInfo,
                    0,
                    lpThreadId
                );*/

            Ref<MeshData> data = ProcessMesh(mesh, scene);

            Entity* entity = nullptr;

            bool entitycreated = false;

            if (parentEntity)
            {
                for (auto const& [key, val] : modelLoadData.assetData)
                {
                    if (val.Name == std::string(mesh->mName.C_Str()) + "_Mesh")
                    {
                        entity = &m_GameScene->CreateAssetEntityWithID(key, *parentEntity, std::string(mesh->mName.C_Str()) + "_Mesh", true, IsAsset);
                        entitycreated = true;
                        break;
                    }
                }

                if(!entitycreated)
                    entity = &m_GameScene->CreateChildEntity(*parentEntity, std::string(mesh->mName.C_Str()) + "_Mesh", IsAsset);
            }
            if (entity)
            {
                entity->AddComponent<MeshComponent>(MeshComponent());
                MeshComponent* meshcomp = &entity->GetComponent<MeshComponent>();
                meshcomp->MeshDrawData = data;
                meshcomp->MaterialIns = m_MaterialIns;
                meshcomp->MaterialPath = modelLoadData.materialPath;
                meshcomp->ID = CreateRef<GUID>(entity->GetGUID());
                //VEL_CORE_INFO("Parent ID: {0}, Child ID: {1}", entity->GetGUID(), parentEntity->GetGUID());
                //VEL_CORE_INFO("ID: {0}, Mesh Name: {1}", *meshcomp->ID, mesh->mName.C_Str());
               // m_IDToMeshData.emplace(entity->GetGUID(), data);
                std::string materialLocation = modelLoadData.materialPath;
                if (modelLoadData.assetData.find(entity->GetAssetID()) != modelLoadData.assetData.end())
                {
                    materialLocation = modelLoadData.assetData[entity->GetAssetID()].MaterialLocation;
                    Ref<Material> material = MaterialSystem::GetMaterial(materialLocation);
                    if (material)
                        meshcomp->MaterialIns = material;
                    meshcomp->MaterialPath = materialLocation;
                }
                m_AssetHandle.push_back({ entity->Name(),entity->GetGUID(),entity->GetAssetID(), materialLocation});
                //MeshRenderer::AddMeshData(entity->GetAssetID(), data);
            }
            m_Meshes.push_back(data);
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            //VEL_CORE_TRACE("Parent : {0}, Child Mesh: {1}", node->mChildren[i]->mParent->mName.C_Str(), node->mChildren[i]->mName.C_Str());
            Entity* entity = nullptr;

            if (m_GameScene != nullptr)
            {
                bool entitycreated = false;
                if (parentEntity)
                {
                    for (auto const& [key, val] : modelLoadData.assetData)
                    {
                        if (val.Name == node->mChildren[i]->mName.C_Str())
                        {
                            entity = &m_GameScene->CreateAssetEntityWithID(key, *parentEntity, node->mChildren[i]->mName.C_Str(), true, IsAsset);
                            entitycreated = true;
                            break;
                        }
                    }

                    if (!entitycreated)
                        entity = &m_GameScene->CreateChildEntity(*parentEntity, node->mChildren[i]->mName.C_Str(), IsAsset);
                }
                else entity = &m_GameScene->CreateEntity(node->mChildren[i]->mParent->mName.C_Str(), IsAsset);
                entity->SetScene(m_GameScene);
                m_AssetHandle.push_back({ entity->Name(),entity->GetGUID(), entity->GetAssetID(), ""});
            }
            ProcessNode(node->mChildren[i], scene, entity);
        }
        SaveAssetFile(GUID(), m_Path + "/" + m_Name + ".vasset", m_AssetHandle);
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
        for (Ref<MeshData> meshData : m_Meshes)
        {
           /* for (Ref<Texture2D> texture : meshData.m_Textures)
            {
                texture->Bind();
            }*/
            if(meshData->m_Textures.size() > 0)
                meshData->m_Textures[0]->Bind();
            Renderer::Submit(shader, meshData->m_VertexArray, transform);
        }
    }
    void Model::DrawMesh(Ref<Shader> shader, const Ref<Material> material, const glm::mat4& transform)
    {
        if (m_Meshes.empty())
            return;
        for (Ref<MeshData> meshData : m_Meshes)
        {
            if (!meshData->m_VertexArray)
            {
                if (meshData->m_Loaded)
                {
                    meshData->m_VertexArray = VertexArray::Create();
                    vel::Ref<vel::VertexBuffer> vertexBuffer;
                    vertexBuffer = vel::VertexBuffer::Create(
                        &meshData->m_Vertices[0],
                        sizeof(vel::Vertices) * meshData->m_Vertices.size()
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
                    meshData->m_VertexArray->AddVertexBuffer(vertexBuffer);
                    Ref<IndexBuffer> ib = IndexBuffer::Create(
                        &meshData->m_Indices[0],
                        meshData->m_Indices.size()
                    );
                    meshData->m_VertexArray->SetIndexBuffer(ib);
                }
                else continue;
            }

            std::vector<Ref<Texture2D>> textures;
            shader->Bind();

            if (m_UseFBXTextures)
            {
                shader->SetBool("useTextureNormal", false);
                shader->SetBool("useTextureSpecular", false);
                shader->SetBool("useTextureEmissive", false);
                shader->SetFloat4("RGBA", material->Diffuse);
                shader->SetFloat4("EMIS", material->Emissive);
                shader->SetFloat4("SPEC", material->Specular);
                shader->SetFloat("SHIN", material->Shininess);
                shader->SetBool("useTextureDiffuse", true);
                Renderer::Submit(shader, meshData->m_VertexArray, meshData->m_Textures, transform);
            }
            else {
                bool useDifMap = false;
                bool useSpeMap = false;
                bool useNorMap = false;
                bool useEmiMap = false;

                if (material && material->DiffuseTexture != nullptr && material->DiffuseTexture->IsLoaded())
                {
                    useDifMap = true;
                }
                shader->SetFloat4("RGBA", material->Diffuse);

                if (material && material->SpecularTexture != nullptr && material->SpecularTexture->IsLoaded())
                {
                    useSpeMap = true;
                }
                shader->SetFloat4("SPEC", material->Specular);

                if (material && material->NormalTexture != nullptr && material->NormalTexture->IsLoaded())
                {
                    useNorMap = true;
                }
                if (material && material->EmissiveTexture != nullptr && material->EmissiveTexture->IsLoaded())
                {
                    useEmiMap = true;
                }
                shader->SetFloat4("EMIS", material->Emissive);
                shader->SetFloat("SHIN", material->Shininess);
                shader->SetFloat("EmissiveIntensity", material->EmissiveIntensity);
                shader->SetBool("useTextureNormal", useNorMap);
                shader->SetBool("useTextureSpecular", useSpeMap);
                shader->SetBool("useTextureDiffuse", useDifMap);
                shader->SetBool("useTextureEmissive", useEmiMap);

                textures.push_back(material->DiffuseTexture);
                textures.push_back(material->NormalTexture);
                textures.push_back(material->SpecularTexture);
                textures.push_back(material->EmissiveTexture);

                Renderer::Submit(shader, meshData->m_VertexArray, textures, transform);
            }
        }
    }
    
    void Model::DrawMesh(Ref<FrameBuffer> buffer, Ref<FrameBuffer> mainbuffer, Ref<Shader> shader, const Ref<Material> material, const glm::mat4& transform)
    {
        if (m_Meshes.empty())
            return;
        for (Ref<MeshData> meshData : m_Meshes)
        {
            if (!meshData->m_VertexArray)
            {
                if (meshData->m_Loaded)
                {
                    meshData->m_VertexArray = VertexArray::Create();
                    vel::Ref<vel::VertexBuffer> vertexBuffer;
                    vertexBuffer = vel::VertexBuffer::Create(
                        &meshData->m_Vertices[0],
                        sizeof(vel::Vertices) * meshData->m_Vertices.size()
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
                    meshData->m_VertexArray->AddVertexBuffer(vertexBuffer);
                    Ref<IndexBuffer> ib = IndexBuffer::Create(
                        &meshData->m_Indices[0],
                        meshData->m_Indices.size()
                    );
                    meshData->m_VertexArray->SetIndexBuffer(ib);
                }
                else continue;
            }

            shader->Bind();

            bool useSpeMap = false;
            bool useNorMap = false;
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
            shader->SetFloat("SHIN", material->Shininess);
            shader->SetBool("useTextureNormal", useNorMap);
            shader->SetBool("useTextureSpecular", useSpeMap);
            shader->SetBool("useTextureDiffuse", true);

            Renderer::Submit(shader, meshData->m_VertexArray, buffer, mainbuffer, transform);
        }
    }
}