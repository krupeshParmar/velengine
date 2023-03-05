#pragma once
#include "Entity.h"
#include "Component.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "LightManager.h"

namespace vel
{
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		std::vector<Ref<Entity>> GetAllEntities();

		void InitCriticalSections();
		void DeleteCriticalSections();

		Ref<Entity> GetEntity(uint32_t id);

		uint32_t CreateEntity(std::string name = "GameObject");

		void AddChild(Entity* parentEntity, Entity* childEntity);

		template<class T>
		bool HasComponent(uint32_t entityID)
		{
			std::map<uint32_t, std::vector<Component*>*>::iterator entityIT = m_EntityMap.find(entityID);
			if (entityIT != m_EntityMap.end())
			{
				std::vector<Component*>* componentList = entityIT->second;
				for (int i = 0; i < componentList->size(); i++)
				{
					T* component = dynamic_cast<T*>(componentList->at(i));
					if (component != nullptr)
						return true;
				}
			}
			return false;
		}

		template<class T>
		T* GetComponentByType(uint32_t entityID)
		{
			std::map<uint32_t, std::vector<Component*>*>::iterator entityIT = m_EntityMap.find(entityID);
			if (entityIT != m_EntityMap.end())
			{
				std::vector<Component*>* componentList = entityIT->second;
				for (int i = 0; i < componentList->size(); i++)
				{
					T* component = dynamic_cast<T*>(componentList->at(i));
					if (component != nullptr)
						return component;
				}
			}
			return nullptr;
		}

		template<class T>
		bool AddComponent(uint32_t entityID, T* newComponent)
		{
			std::map<uint32_t, std::vector<Component*>*>::iterator entityIT = m_EntityMap.find(entityID);
			if (entityIT != m_EntityMap.end())
			{
				std::vector<Component*>* componentList = entityIT->second;
				if (HasComponent<T>(entityID))
				{
					return false;
				}
				LightComponent* component = dynamic_cast<LightComponent*>(newComponent);
				if (component != nullptr)
				{
					LightManager::AddNewLightInfo(component);
				}

				componentList->push_back(newComponent);
				return true;
			}
			return false;
		}

		template<class T>
		T* RemoveComponent(uint32_t entityID)
		{
			std::map<uint32_t, std::vector<Component*>*>::iterator entityIT = m_EntityMap.find(entityID);
			if (entityIT != m_EntityMap.end())
			{
				std::vector<Component*>* componentList = entityIT->second;
				for (std::vector<Component*>::iterator it = componentList->begin();
					it != componentList->end(); it++)
				{
					T* component = dynamic_cast<T*>(*it);
					if (component != nullptr)
					{
						componentList->erase(it);
						return *it;
					}
				}
			}
			return nullptr;
		}


	private:
		std::map<uint32_t, std::vector<Component*>* > m_EntityMap;
		Ref<std::vector<Ref<Entity>>> m_EntityList;
		uint32_t m_NextEntityID;
	};
}