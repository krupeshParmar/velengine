#include "velpch.h"

#include "EntityManager.h"
// controls acces to the vector of m_Meshes
CRITICAL_SECTION AddEntityCheck_Lock;

namespace vel
{

	EntityManager::EntityManager()
	{
		InitializeCriticalSection(&AddEntityCheck_Lock);
		m_EntityList = CreateRef< std::vector<Ref<Entity>>>();
		m_NextEntityID = 0;
		LightManager();
	}
	EntityManager::~EntityManager()
	{
		DeleteCriticalSection(&AddEntityCheck_Lock);
	}

	std::vector<Ref<Entity>> EntityManager::GetAllEntities()
	{
		return *m_EntityList;
	}

	Ref<Entity> EntityManager::GetEntity(uint32_t id)
	{
		for (
			std::vector<Ref<Entity>>::iterator it = m_EntityList->begin();
			it != m_EntityList->end();
			it++
			)
		{
			Ref<Entity> entity = *it;
			if (entity->GetID() == id)
			{
				return entity;
			}
		}
		return nullptr;
	}

	uint32_t EntityManager::CreateEntity(std::string name)
	{
		Ref<Entity> entity = CreateRef<Entity>();
		entity->name = name;

		entity->SetID(m_NextEntityID++);

		std::vector<Component*>* components = new std::vector<Component*>();
		m_EntityMap.emplace(entity->GetID(), components);
		AddComponent(entity->GetID(), new TransformComponent());

		EnterCriticalSection(&AddEntityCheck_Lock);

		m_EntityList->push_back(entity);

		LeaveCriticalSection(&AddEntityCheck_Lock);

		return entity->GetID();
	}

	void EntityManager::AddChild(Entity* parentEntity, Entity* childEntity)
	{
		parentEntity->AddChild(childEntity);
		childEntity->SetParent(parentEntity);
	}

	void EntityManager::InitCriticalSections()
	{
	}
	void EntityManager::DeleteCriticalSections()
	{
	}
}