#pragma once
#include <string>
#include <vector>
#include <map>
#include "vel/Renderer/Model.h"

namespace vel
{
	class MeshRenderer
	{
	public:
		MeshRenderer();
		~MeshRenderer();

		static Ref<Model> LoadMesh(std::string& path, bool useTextures, bool loadAsync, Ref<EntityManager> entityManger = nullptr);
		static bool HasMesh(std::string& path);
		static Ref<Model> GetMesh(std::string& path);

	private:
		static std::map<std::string, Ref<Model>> m_PathToModel;

	};
}