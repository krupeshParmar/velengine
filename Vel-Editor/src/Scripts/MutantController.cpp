#include "MutantController.h"
#include "States/MutantBaseState.h"
#include "States/MutantRunState.h"
#include "States/MutantWalkState.h"
#include "States/MutantAttackState.h"
#include "States/MutantIdleState.h"

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
	if (targetTransform && selfTransform)
	{
		currentState->Update(this, ts);
		float distance = glm::distance(selfTransform->Translation, targetTransform->Translation);

		// Within Range
		if (distance <= 2.f)
		{
			if (currentState != attackState)
			{
				TransitionToState(attackState);
			}
		}

		// Outside Range but close
		else if (distance > 2.f && distance < 5.f)
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
