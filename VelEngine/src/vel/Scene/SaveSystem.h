#pragma once
#include "EntityManager.h"
#include <string>
#include <pugixml.hpp>

namespace vel
{

	bool SaveMaterial(std::string materialPath, Ref<Material>);
	Ref<Material> LoadMaterial(std::string materialPath);
	/// <summary>
	/// Save Scene File
	/// </summary>
	/// <param name="filename">Scene Name</param>
	/// <param name="entityManager">To get info of all entities</param>
	/// <returns>If saved successfully</returns>
	bool SaveSceneFile(std::string filename, Ref<EntityManager> entityManager);

	/// <summary>
	/// Loads Scene file
	/// </summary>
	/// <param name="filename">Name of the save file</param>
	/// <param name="entityManager">To get info of all entities</param>
	/// <returns>If loaded successfully</returns>
	bool LoadSceneFile(std::string filename, Ref<EntityManager> entityManager);
}