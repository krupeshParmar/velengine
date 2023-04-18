#include "MutantController.h"
#include "States/MutantBaseState.h"
#include "States/MutantRunState.h"
#include "States/MutantWalkState.h"
#include "States/MutantAttackState.h"
#include "States/MutantIdleState.h"
#include "PlayerController.h"

void MutantController::OnCreate()
{
	FindAnimator(GetScene()->GetEntityWithGUID(GetComponent<vel::IDComponent>().ID));
	auto aiView = GetScene()->GetAllEntitiesWith<vel::AIComponent>();
	for (entt::entity entity : aiView)
	{
		vel::Entity aiEntity = vel::Entity{ entity, GetScene() };
		selfTransform = &aiEntity.Transform();
	}
	auto ccView = GetScene()->GetAllEntitiesWith<vel::CharacterControllerComponent>();
	for (entt::entity entity : ccView)
	{
		vel::Entity ccEntity = vel::Entity{ entity, GetScene() };
		targetTransform = &ccEntity.Transform();
		break;
	}
	selfHealth = &GetComponent<HealthComponent>();
	walkState = new MutantWalkState();
	runState = new MutantRunState();
	attackState = new MutantAttackState();
	idleState = new MutantIdleState();
	TransitionToState(idleState);
}

void MutantController::OnDestroy()
{
}

void MutantController::OnUpdate(vel::Timestep ts)
{
	if (dead)
		return;
	if (selfHealth)
	{
		if (selfHealth->health <= 0.f)
		{
			animatorComponent->PlayAnimation(5);
			dead = true;
			return;
		}
	}
	if (targetTransform && selfTransform)
	{
		if (!targetController)
		{
			vel::Entity mariaEntity = GetScene()->TryGetEntityWithTag("Maria2");
			if (mariaEntity)
			{
				ScriptableEntity* script = mariaEntity.GetComponent<vel::NativeScriptComponent>().Instance;
				targetController = (PlayerController*)script;
			}
		}

		currentState->Update(this, ts);
		float distance = glm::distance(selfTransform->Translation, targetTransform->Translation);

		// Within Range
		if (distance <= 1.5f)
		{
			if (currentState != attackState)
			{
				TransitionToState(attackState);
			}
		}

		// Outside Range but close
		else if (distance > 1.5f && distance < 7.f)
		{
			if (currentState != walkState)
			{
				TransitionToState(walkState);
			}
		}

		// Outside Range but very far
		else
		{
			if (currentState != runState)
			{
				TransitionToState(runState);
			}
		}
	}
}

void MutantController::TransitionToState(MutantBaseState* state)
{
	currentState = state;
	currentState->EnterState(this);
}

void MutantController::FindAnimator(vel::Entity entity)
{
	if (entity.HasComponent<vel::AnimatorComponent>())
	{
		animatorComponent = &entity.GetComponent<vel::AnimatorComponent>();
	}
	if (entity.Children().size() > 0)
	{
		for (vel::GUID guid : entity.Children())
		{
			FindAnimator(GetScene()->GetEntityWithGUID(guid));
		}
	}
}
