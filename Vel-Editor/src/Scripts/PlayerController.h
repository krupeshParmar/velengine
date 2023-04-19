#pragma once
#include "Vel.h"
#include <DistanceConstraint.h>
#include <glm/gtc/quaternion.hpp>
#include "CameraController.h"

enum AnimState
{
	Idle,
	Idle2,
	Idle3,
	Walk,
	Run,
	Blocking,
	Slash2,
	Slash,
	Cast,
	Spell,
	Impact,
	Death,
	FlyingDeath,
	None
};
struct HealthComponent;
class PlayerController : public vel::ScriptableEntity
{
public:
	
	template<typename T>
	void FindComponent(vel::Entity entity, std::vector<T*>& list)
	{
		if (entity.HasComponent<T>())
		{
			list.push_back(&entity.GetComponent<T>());
		}
		if (entity.Children().size() > 0)
		{
			for (vel::GUID guid : entity.Children())
			{
				FindComponent<T>(GetScene()->GetEntityWithGUID(guid), list);
			}
		}
	}
	virtual void OnCreate() override;
	virtual void OnUpdate(vel::Timestep ts) override;
	virtual void OnDestroy() override;
	const void TakeDamage(float damage);
	const void IncrementXP();
	const float GetXP();
	const glm::vec3& GetPosition();
private:
	glm::vec3 m_Position = glm::vec3(0.f);
	std::vector<vel::AnimatorComponent*> animatorsList;
	physics::iCharacterController* characterController;
	std::vector < vel::RigidbodyComponent*> enemyRigidBodyList;
	AnimState state = None;
	CameraController* cameraController = nullptr;
	HealthComponent* enemyHealth = nullptr;
	HealthComponent* selfHealth = nullptr;
	vel::TransformComponent* enemyTransform = nullptr;
	vel::TransformComponent* selfTransform = nullptr;
	vel::RigidbodyComponent* globeBody = nullptr;
	const float RUN_SPEED = 5.f;
	const float WALK_SPEED = 2.5f;
	const float ROTATION_SPEED = 0.5f;
	float speed = WALK_SPEED;
	bool attacked = false;
	float damageTaken = 0.f;
};