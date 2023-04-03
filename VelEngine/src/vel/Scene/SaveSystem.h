#pragma once
#include "EntityManager.h"
#include <string>
#include <pugixml.hpp>
#include "vel/Renderer/Material.h"
#include "Scene.h"
#include "Component.h"

namespace vel
{
	bool SaveMaterial(std::string materialPath, Ref<Material>);
	Ref<Material> GetMaterialFromSaveFile(std::string materialPath);

	bool SaveSceneFile(std::string filename, Scene* scene);
	bool LoadSceneFile(std::string filename, Scene* scene, bool async = true);

	bool SaveAssetFile(GUID assetID,std::string filename, std::vector<Asset> assets);

	std::map<GUID, Asset> LoadAssetFile(std::string filename);

	/// <summary>
	/// Save Scene File
	/// </summary>
	/// <param name="filename">Scene Name</param>
	/// <param name="entityManager">To get info of all entities</param>
	/// <returns>If saved successfully</returns>
	//bool SaveSceneFile(std::string filename, Ref<EntityManager> entityManager);

	/// <summary>
	/// Loads Scene file
	/// </summary>
	/// <param name="filename">Name of the save file</param>
	/// <param name="entityManager">To get info of all entities</param>
	/// <returns>If loaded successfully</returns>
	//bool LoadSceneFile(std::string filename, Ref<EntityManager> entityManager, bool async = true);
}