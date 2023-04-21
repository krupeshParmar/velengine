#pragma once
#include "Vel.h"
#include "HealthComponent.h"
#include <chrono>

class MutantBaseState;
class MutantRunState;
class MutantWalkState;
class MutantAttackState;
class MutantIdleState;
class PlayerController;

class MutantController : public vel::ScriptableEntity
{
public:
	vel::TransformComponent* selfTransform = nullptr;
	vel::TransformComponent* targetTransform = nullptr;
	vel::AnimatorComponent* animatorComponent = nullptr;
	vel::RigidbodyComponent* rigidBodyComponent = nullptr;
	PlayerController* targetController = nullptr;
	vel::ParticlesComponent particlesData;

	virtual void OnCreate() override;

	virtual void OnDestroy() override;

	virtual void OnUpdate(vel::Timestep ts) override;

private:
	void TransitionToState(MutantBaseState* state);

	void FindAnimator(vel::Entity entity);

private:
	HealthComponent* selfHealth = nullptr;
	MutantBaseState* currentState = nullptr;
	MutantRunState* runState = nullptr;
	MutantWalkState* walkState = nullptr;
	MutantAttackState* attackState = nullptr;
	MutantIdleState* idleState = nullptr;
	std::chrono::steady_clock::time_point jointsFoundTime;
	bool dead = false;
	bool jointsAdded = false;
	float cachedY = 0.5f;
	float lastSeenHealth = 100.f;
};