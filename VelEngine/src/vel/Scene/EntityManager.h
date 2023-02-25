#pragma once
#include "Entity.h"
#include "Component.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace vel
{
	class EntityManager
	{
	public:
		std::vector<Entity*> GetAllEntities() const
		{
			return m_EntityList;
		}

		void CreateEntity(std::string name = "GameObject")
		{
			Entity* entity = new Entity();
			entity->name = name;
			entity->SetID(m_NextEntityID++);

			m_EntityList.push_back(entity);
			std::vector<Component*>* components = new std::vector<Component*>();
			m_EntityMap.emplace(entity->GetID(), components);
			if (AddComponent(entity->GetID(), new TransformComponent()))
			{
				int breakMeHere = 0;
			}
		}

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
		std::vector<Entity*> m_EntityList;
		uint32_t m_NextEntityID = 0;
	};
}