#include "velpch.h"
#include <cstring>
#include "vel/Core/GUID.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "vel/Renderer/Renderer.h"
#include "Component.h"
#include "Entity.h"

CRITICAL_SECTION MeshMapList_Lock;

namespace vel
{
	std::map<std::string, Ref<Model>> MeshRenderer::m_PathToModel = std::map<std::string, Ref<Model>>();
	std::map<GUID, Ref<MeshData>> MeshRenderer::m_IdToMeshData = std::map<GUID, Ref<MeshData>>();
	MeshRenderer::MeshRenderer()
	{
		InitializeCriticalSection(&MeshMapList_Lock);
		//m_PathToModel.clear();
	}

	MeshRenderer::~MeshRenderer()
	{
		DeleteCriticalSection(&MeshMapList_Lock);
	}

	Entity* MeshRenderer::LoadMesh(ModelLoadData data)
	{
		/*for (int x = 0; x < data.source.size(); x++)
			data.source[x] = tolower(data.source[x]);*/

		/*if (MeshRenderer::HasMesh(data.source))
			return m_PathToModel.find(data.source)->second;*/
		Ref<Model> model = CreateRef<Model>(data);
		if (!data.parentEntity->HasComponent<AssetComponent>())
		{
			data.parentEntity->AddComponent
				<AssetComponent>(AssetComponent(model->GetFullPath(), model));
		}
		else
		{
			AssetComponent& asset = data.parentEntity->GetComponent<AssetComponent>();
			asset.ModelInstance = model;
		}

		//EnterCriticalSection(&MeshMapList_Lock);
		//m_PathToModel.emplace(data.source, model);
		//LeaveCriticalSection(&MeshMapList_Lock);
		return model->m_ModelPrefab;
	}

	Ref<Model> MeshRenderer::LoadMesh(std::string& path, bool useTextures, bool loadAsync, Scene* scene)
	{
		for (int x = 0; x < path.size(); x++)
			path[x] = tolower(path[x]);

		if (MeshRenderer::HasMesh(path))
			return m_PathToModel.find(path)->second;
		Ref<Model> model = CreateRef<Model>(path, useTextures, loadAsync, scene);

		//EnterCriticalSection(&MeshMapList_Lock);
		m_PathToModel.emplace(path, model);
		//LeaveCriticalSection(&MeshMapList_Lock);
		return model;
	}

	bool MeshRenderer::HasMesh(std::string& path)
	{
		return m_PathToModel.find(path) != m_PathToModel.end();
	}

	Ref<Model> MeshRenderer::GetMesh(std::string& path)
	{
		std::map<std::string,Ref<Model>>::iterator modelIT = m_PathToModel.find(path);
		if (modelIT != m_PathToModel.end())
			return modelIT->second;
		return nullptr;
	}
	void MeshRenderer::DrawMesh(MeshComponent& meshComponent, glm::mat4& transform)
	{
		bool useDifMap = false;
		bool useSpeMap = false;
		bool useNorMap = false;
		bool useEmiMap = false;
		Ref<Material> material = meshComponent.MaterialIns;
		Ref<Shader> shader = meshComponent.shader;
		std::map<GUID, Ref<MeshData>>::iterator it;
		Ref<MeshData> meshData = meshComponent.MeshDrawData;
		std::vector<Ref<Texture2D>> textures;

		if (meshData)
		{
			if (!meshData->m_VertexArray)
			{
				if (meshData->m_Loaded)
				{
					meshData->m_VertexArray = VertexArray::Create();
					vel::Ref<vel::VertexBuffer> vertexBuffer;
					if (meshData->m_UseBones)
					{
						for (const Vertices& vertex : meshData->m_Vertices)
						{
							for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
							{
								if (vertex.vBoneID[i] >= 100)
								{
									VEL_CORE_ASSERT(0, "");
								}
							}
						}
					}
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
							{ vel::ShaderDataType::Int4, "vBoneID"},
							{ vel::ShaderDataType::Float4, "vBoneWeight"}
						});
					meshData->m_VertexArray->AddVertexBuffer(vertexBuffer);
					Ref<IndexBuffer> ib = IndexBuffer::Create(
						&meshData->m_Indices[0],
						meshData->m_Indices.size()
					);
					meshData->m_VertexArray->SetIndexBuffer(ib);
				}
				//else continue;
			}

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
			shader->SetBool("u_UseBone", meshData->m_UseBones);

			textures.push_back(material->DiffuseTexture);
			textures.push_back(material->NormalTexture);
			textures.push_back(material->SpecularTexture);
			textures.push_back(material->EmissiveTexture);

			Renderer::Submit(shader, meshData->m_VertexArray, textures, transform);
		}
	}
	void MeshRenderer::AddMeshData(GUID id, Ref<MeshData> meshData)
	{
		m_IdToMeshData.emplace(id, meshData);
	}
	bool MeshRenderer::HasMeshData(GUID id)
	{
		return m_IdToMeshData.find(id) != m_IdToMeshData.end();
	}
	Ref<MeshData> MeshRenderer::GetMeshData(GUID id)
	{
		std::map<GUID, Ref<MeshData>>::iterator meshDataIT = m_IdToMeshData.find(id);
		if (meshDataIT != m_IdToMeshData.end())
			return meshDataIT->second;
		return nullptr;
	}
}