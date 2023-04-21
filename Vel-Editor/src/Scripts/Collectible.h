#pragma once

#include "Vel.h"
#include <chrono>
#include "PlayerController.h"

class Collectible : public vel::ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
		collectibleEntityPrefab = GetScene()->TryGetEntityWithTag("collectible");
		collectibleEntity = &GetScene()->DuplicateEntity(collectibleEntityPrefab, GetEntity());

		if (collectibleEntity->HasComponent<vel::LightComponent>())
		{
			vel::LightComponent& light = collectibleEntity->GetComponent<vel::LightComponent>();
			light.Enabled = true;
		}
		
		vel::TransformComponent* transform = &collectibleEntity->Transform();
		transform->enabled = true;
		GetEntity().GetComponentInChild<vel::MeshComponent>(meshComponent);
		selfTransform = &GetComponent<vel::TransformComponent>();

		srand(std::chrono::steady_clock::now().time_since_epoch().count());
		int x = (rand() % 40) - 20;
		int z = (rand() % 40) - 20;

		selfTransform->Translation.x = x;
		selfTransform->Translation.z = z;
	}
	virtual void OnDestroy() override
	{

	}
	virtual void OnUpdate(vel::Timestep ts) override
	{
		if (collected)
			return;

		if (decreaseNow)
		{
			meshComponent.MaterialIns->EmissiveIntensity -= 0.1f;
			if (meshComponent.MaterialIns->EmissiveIntensity < 2.f)
			{
				decreaseNow = false;
			}
		}
		else
		{
			meshComponent.MaterialIns->EmissiveIntensity += 0.1f;
			if (meshComponent.MaterialIns->EmissiveIntensity > 20.f)
			{
				decreaseNow = true;
			}
		}
		if (!playerController)
		{
			vel::Entity mariaEntity = GetScene()->TryGetEntityWithTag("Maria2");
			if (mariaEntity)
			{
				ScriptableEntity* script = mariaEntity.GetComponent<vel::NativeScriptComponent>().Instance;
				playerController = (PlayerController*)script;
			}
		}
		if (playerController)
		{
			glm::vec3 playerPosition = playerController->GetPosition();
			playerPosition.y = selfTransform->Translation.y;
			float distance = glm::distance(playerPosition, selfTransform->Translation);
			if (distance < 0.7f)
			{
				playerController->IncrementXP();
				collected = true;
			}
		}
	}

	void Resposition()
	{
		if (!selfTransform)
			return;
		srand(std::chrono::steady_clock::now().time_since_epoch().count());
		int x = (rand() % 40) - 20;
		int z = (rand() % 40) - 20;

		selfTransform->Translation.x = x;
		selfTransform->Translation.z = z;
	}

	bool collected = false;
private:
	PlayerController* playerController = nullptr;
	vel::Entity collectibleEntityPrefab;
	vel::Entity* collectibleEntity = nullptr;
	vel::TransformComponent* selfTransform = nullptr;
	vel::MeshComponent meshComponent;
	bool decreaseNow = false;
};