#pragma once
#include "vel/Core/Core.h"
#include <string>
#include <vector>
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "VertexArray.h"
#include <glm/glm.hpp>
#include "Material.h"
#include "FrameBuffer.h"
#include <map>
namespace Assimp
{
	class Importer;
}

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;


namespace vel
{
	class Entity;
	class Scene;
	class GUID;
	struct Asset;
	struct ModelLoadData
	{
		std::string source;
		bool useTextures;
		bool loadAsync;
		Scene* gameScene;
		Ref<Material> material;
		std::string materialPath;
		Entity* parentEntity;
		std::map<GUID,Asset> assetData;
	};

	struct MeshData;

	struct BoneInfo;

	class Model
	{
	public:

		Entity* m_ModelPrefab;
		std::map<GUID, Ref<MeshData>> m_IDToMeshData;

		Model(ModelLoadData data);
		Model(std::string source, bool useTextures, bool loadAsync);
		Model(std::string source, bool useTextures, bool loadAsync, Scene* gameScene);
		~Model();
		MeshData GetMeshData();
		std::vector<Asset> GetAssets() { return m_AssetHandle; }
		std::string GetFullPath() { return m_Path + "/" + m_Name + ".vasset"; }

		void DrawMesh(Ref<Shader> shader, const glm::mat4& transform);
		void DrawMesh(Ref<Shader> shader,const Ref<Material> material, const glm::mat4& transform);
		
		/// <summary>
		/// MESSSSSSS
		/// </summary>
		/// <param name="buffer">Render buffer</param>
		/// <param name="shader">tempshader</param>
		/// <param name="material">quad material</param>
		/// <param name="transform">transform</param>
		void DrawMesh(Ref<FrameBuffer> buffer, Ref<FrameBuffer> mainbuffer, Ref<Shader> shader, const Ref<Material> material, const glm::mat4& transform);
		inline std::vector<Ref<MeshData>> GetWholeMesh() const { return m_Meshes; }
		std::string GetPath() const { return m_Path; }
	private:
		void ProcessNode(aiNode* node, const aiScene* scene, Entity* parentEntity);
		Ref<MeshData> ProcessMesh(aiMesh* aimesh, const aiScene* scene);
		std::vector<Ref<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typesname);
		
		void m_InitCriticalSections(void);
		void m_DeleteCriticalSections(void);
		void SetVertexBoneDataToDefault(Vertices* vertex);
		void SetVertexBoneData(Vertices* vertex, int boneID, float weight);
		void ExtractBoneWeightForVertices(Ref<MeshData> meshData, aiMesh* mesh, const aiScene* scene);

	private:
		ModelLoadData modelLoadData;
		bool m_UseFBXTextures;
		bool m_LoadAsync = false;
		Scope<Assimp::Importer> m_Importer;
		std::string m_Path, m_Name;
		std::vector<Ref<MeshData>> m_Meshes;
		std::vector<Ref<Texture2D>> m_TexturesLoaded;
		Scene* m_GameScene;
		std::vector<Asset> m_AssetHandle;
		Ref<Material> m_MaterialIns;
		std::string m_MaterialPath;
		std::unordered_map<std::string, BoneInfo>* m_BoneInfoMap;
		int m_BoneCounter = 0;
	};

}