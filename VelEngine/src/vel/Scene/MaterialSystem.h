#pragma once
#include <string>
#include <vector>
#include <map>
#include "vel/Renderer/Material.h"

namespace vel
{
	class MaterialSystem
	{
	public:
		MaterialSystem();
		~MaterialSystem();

		static Ref<Material> LoadMaterial(std::string& path, Ref<Material> material);
		static bool HasMaterial(std::string& path);
		static Ref<Material> GetMaterial(std::string& path);

	private:
		static std::map<std::string, Ref<Material>> m_PathToMaterial;
		static Ref<Material> defaultMaterial;

	};
}