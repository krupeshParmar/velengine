#pragma once
#include "Vel.h"
#include "Collectible.h"

struct CollectibleData
{
	vel::NativeScriptComponent* script;
	vel::Entity* entity;
};

class CollectibleManager : public vel::ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
		for (int i = 0; i < 5; i++)
			CreateNewCollectible();
	}
	virtual void OnDestroy() override
	{

	}
	virtual void OnUpdate(vel::Timestep ts) override
	{
		std::vector<CollectibleData>::iterator collectibleIT = m_Collectibles.begin();
		for (;
			collectibleIT != m_Collectibles.end(); collectibleIT++)
		{
			CollectibleData data = *collectibleIT;
			if (data.script->Instance)
			{
				Collectible* collectible = (Collectible*)data.script->Instance;
				if (collectible->collected)
				{
					collectible->collected = false;
					collectible->Resposition();
				}
			}
		}
	}

	void CreateNewCollectible()
	{
		CollectibleData data;
		vel::NativeScriptComponent* collectible;
		vel::Entity* entity = &GetScene()->CreateEntity("collectible_" + std::to_string(collectibleCounter));
		if (entity)
		{
			entity->AddComponent<vel::NativeScriptComponent>().Bind<Collectible>();
			collectible = &entity->GetComponent<vel::NativeScriptComponent>();
			data.entity = entity;
			data.script = collectible; 
			m_Collectibles.push_back(data);
			collectibleCounter++;
		}
	}

private:
	int collectibleCounter = 1;
	std::vector<CollectibleData> m_Collectibles;
};