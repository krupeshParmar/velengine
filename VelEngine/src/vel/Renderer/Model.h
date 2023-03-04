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
		Model(std::string source, bool useTextures);
		MeshData GetMeshData();
		void DrawMesh(Ref<Shader> shader, const glm::mat4& transform);
		void DrawMesh(Ref<Shader> shader,const Ref<Material> material, const glm::mat4& transform);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		//MeshData ProcessMesh(aiMesh* aimesh, const aiScene* scene);
		std::vector<Ref<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typesname);
	

	private:
		bool m_UseFBXTextures;
		Ref<Assimp::Importer> m_Importer;
		std::string m_Path;
		std::vector<MeshData> m_Meshes;
		std::vector<Ref<Texture2D>> m_TexturesLoaded;
	};

}