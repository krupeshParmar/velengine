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
	struct MeshData
	{
		std::vector<Vertices> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Ref<Texture2D>> m_Textures;

		bool m_Loaded = false;

		Ref<VertexArray> m_VertexArray;
	};

	class Model
	{
	public:
		Model(std::string source, bool useTextures, bool loadAsync);
		~Model();
		MeshData GetMeshData();
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
		void ProcessNode(aiNode* node, const aiScene* scene);
		Ref<MeshData> ProcessMesh(aiMesh* aimesh, const aiScene* scene);
		std::vector<Ref<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typesname);
		
		void m_InitCriticalSections(void);
		void m_DeleteCriticalSections(void);

	private:
		bool m_UseFBXTextures;
		bool m_LoadAsync = false;
		Ref<Assimp::Importer> m_Importer;
		std::string m_Path, m_Name;
		std::vector<Ref<MeshData>> m_Meshes;
		std::vector<Ref<Texture2D>> m_TexturesLoaded;
	};

}