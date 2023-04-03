#include "velpch.h"
#include "MaterialSystem.h"

namespace vel
{
	std::map<std::string, Ref<Material>> MaterialSystem::m_PathToMaterial = std::map<std::string, Ref<Material>>();
	Ref<Material> MaterialSystem::defaultMaterial = CreateRef<Material>();
	
	MaterialSystem::MaterialSystem()
	{

	}
	MaterialSystem::~MaterialSystem()
	{
	}
	Ref<Material> MaterialSystem::LoadMaterial(std::string& path, Ref<Material> material)
	{
		for (int x = 0; x < path.size(); x++)
			path[x] = tolower(path[x]);

		if (material)
		{
			m_PathToMaterial.emplace(path, material);
			return material;
		}
		return defaultMaterial;
	}
	bool MaterialSystem::HasMaterial(std::string& path)
	{
		return m_PathToMaterial.find(path) != m_PathToMaterial.end();
	}
	Ref<Material> MaterialSystem::GetMaterial(std::string& path)
	{
		std::map<std::string, Ref<Material>>::iterator matIT = m_PathToMaterial.find(path);
		if (matIT != m_PathToMaterial.end())
			return matIT->second;
		return defaultMaterial;
	}
}
