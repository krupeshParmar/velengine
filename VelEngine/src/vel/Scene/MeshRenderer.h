#pragma once
#include <string>
#include <vector>
#include <map>
#include "vel/Renderer/Model.h"

namespace vel
{
	class GUID;
	class Scene;
	class MeshComponent;
	class MeshData;
	class MeshRenderer
	{
	public:
		MeshRenderer();
		~MeshRenderer();

		static Entity* LoadMesh(ModelLoadData data);
		static Ref<Model> LoadMesh(std::string& path, bool useTextures, bool loadAsync, Scene* scene);
		static bool HasMesh(std::string& path);
		static Ref<Model> GetMesh(std::string& path);
		static void DrawMesh(MeshComponent& meshComponent, glm::mat4& transform);
		static void AddMeshData(GUID id, Ref<MeshData> meshData);
		static bool HasMeshData(GUID id);
		static Ref<MeshData> GetMeshData(GUID id);

	private:
		static std::map<std::string, Ref<Model>> m_PathToModel;
		static std::map<GUID, Ref<MeshData>> m_IdToMeshData;

	};
}