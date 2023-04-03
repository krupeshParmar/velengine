#include "velpch.h"
//
//#include "EntityManager.h"
//// controls acces to the vector of m_Meshes
//CRITICAL_SECTION AddEntityCheck_Lock;
//
//namespace vel
//{
//
//	EntityManager::EntityManager()
//	{
//		InitializeCriticalSection(&AddEntityCheck_Lock);
//		m_EntityList = CreateRef< std::vector<Ref<Entity>>>();
//		m_NextEntityID = 0;
//		LightManager();
//	}
//	EntityManager::~EntityManager()
//	{
//		DeleteCriticalSection(&AddEntityCheck_Lock);
//	}
//
//	std::vector<Ref<Entity>> EntityManager::GetAllEntities()
//	{
//		return *m_EntityList;
//	}
//
//	Ref<Entity> EntityManager::GetEntity(uint32_t id)
//	{
//		for (
//			std::vector<Ref<Entity>>::iterator it = m_EntityList->begin();
//			it != m_EntityList->end();
//			it++
//			)
//		{
//			Ref<Entity> entity = *it;
//			if (entity->GetID() == id)
//			{
//				return entity;
//			}
//		}
//		return nullptr;
//	}
//
//	uint32_t EntityManager::CreateEntity(std::string name)
//	{
//		Ref<Entity> entity = CreateRef<Entity>();
//		entity->name = name;
//
//		entity->SetID(m_NextEntityID++);
//		EntityData* entityData = new EntityData();
//		
//
//		entityData->ComponentsList = new std::vector<Component*>();
//		entityData->ChildrenList = new std::vector<uint32_t>();
//
//		m_EntityMap.emplace(entity->GetID(), entityData);
//		AddComponent(entity->GetID(), new TransformComponent());
//
//		EnterCriticalSection(&AddEntityCheck_Lock);
//
//		m_EntityList->push_back(entity);
//
//		LeaveCriticalSection(&AddEntityCheck_Lock);
//
//		return entity->GetID();
//	}
//
//	void EntityManager::CreateEntity(uint32_t id)
//	{
//		Ref<Entity> entity = CreateRef<Entity>();
//		entity->name = "GameObject";
//
//		entity->SetID(id);
//		while (m_NextEntityID < id)
//			m_NextEntityID++;
//
//
//		EntityData* entityData = new EntityData();
//
//		entityData->ComponentsList = new std::vector<Component*>();
//		entityData->ChildrenList = new std::vector<uint32_t>();
//
//		m_EntityMap.emplace(entity->GetID(), entityData);
//
//		AddComponent(entity->GetID(), new TransformComponent());
//
//		EnterCriticalSection(&AddEntityCheck_Lock);
//
//		m_EntityList->push_back(entity);
//
//		LeaveCriticalSection(&AddEntityCheck_Lock);
//
//		return;
//	}
//
//	void EntityManager::AddChild(uint32_t parentID,uint32_t childID)
//	{
//		Ref<Entity> child = GetEntity(childID);
//		Ref<Entity> parent = GetEntity(parentID);
//		child->SetParent(parentID);
//		for (
//			std::vector<Ref<Entity>>::iterator it = m_EntityList->begin();
//			it != m_EntityList->end();
//			it++
//			)
//		{
//			Ref<Entity> entity = *it;
//			if (entity->GetID() == parentID)
//			{
//				EntityData* data = m_EntityMap[parentID];
//				data->ChildrenList->push_back(childID);
//			}
//		}
//	}
//
//	EntityData* EntityManager::GetEntityData(uint32_t id)
//	{
//		for (
//			std::map<uint32_t, EntityData*>::iterator  it = m_EntityMap.begin();
//			it != m_EntityMap.end();
//			it++
//			)
//		{
//			if (it->first == id)
//			{
//				return it->second;
//			}
//		}
//
//		return nullptr;
//	}
//
//	void EntityManager::InitCriticalSections()
//	{
//	}
//	void EntityManager::DeleteCriticalSections()
//	{
//	}
//}