#include "velpch.h"
#include <cstring>
#include "MeshRenderer.h"
CRITICAL_SECTION MeshMapList_Lock;

namespace vel
{
	std::map<std::string, Ref<Model>> MeshRenderer::m_PathToModel = std::map<std::string, Ref<Model>>();
	MeshRenderer::MeshRenderer()
	{
		InitializeCriticalSection(&MeshMapList_Lock);
		//m_PathToModel.clear();
	}

	MeshRenderer::~MeshRenderer()
	{
		DeleteCriticalSection(&MeshMapList_Lock);
	}

	Ref<Model> MeshRenderer::LoadMesh(std::string& path, bool useTextures, bool loadAsync, Ref<EntityManager> entityManager)
	{
		for (int x = 0; x < path.size(); x++)
			path[x] = tolower(path[x]);

		if (MeshRenderer::HasMesh(path))
			return m_PathToModel.find(path)->second;
		Ref<Model> model = CreateRef<Model>(path, useTextures, loadAsync);

		//EnterCriticalSection(&MeshMapList_Lock);
		m_PathToModel.emplace(path, model);
		//LeaveCriticalSection(&MeshMapList_Lock);
		return model;
	}

	bool MeshRenderer::HasMesh(std::string& path)
	{
		return m_PathToModel.find(path) != m_PathToModel.end();
	}

	Ref<Model> MeshRenderer::GetMesh(std::string& path)
	{
		std::map<std::string,Ref<Model>>::iterator modelIT = m_PathToModel.find(path);
		if (modelIT != m_PathToModel.end())
			return modelIT->second;
		return nullptr;
	}
}