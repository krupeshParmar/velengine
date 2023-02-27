#pragma once
#include "vel/Core/Core.h"
#include <string>
#include <vector>
#include "Buffer.h"
#include "Texture.h"
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
	};

	class Model
	{
	public:
		Model(std::string source);
		MeshData GetMeshData();

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		MeshData ProcessMesh(aiMesh* aimesh, const aiScene* scene);
		std::vector<Ref<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typesname);


	private:

		Ref<Assimp::Importer> m_Importer;
		std::string m_Path;
		std::vector<MeshData> m_Meshes;
		std::vector<Ref<Texture2D>> m_TexturesLoaded;
	};

}