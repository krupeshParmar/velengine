#include "MutantController.h"
#include "States/MutantBaseState.h"
#include "States/MutantRunState.h"
#include "States/MutantWalkState.h"
#include "States/MutantAttackState.h"
#include "States/MutantIdleState.h"
#include "PlayerController.h"
#include <BoxShape.h>

void MutantController::OnCreate()
{
	jointsFoundTime = std::chrono::steady_clock::now();
	FindAnimator(GetScene()->GetEntityWithGUID(GetComponent<vel::IDComponent>().ID));

	selfTransform = &GetComponent<vel::TransformComponent>();
	cachedY = selfTransform->Translation.y;
	auto ccView = GetScene()->GetAllEntitiesWith<vel::CharacterControllerComponent>();
	for (entt::entity entity : ccView)
	{
		vel::Entity ccEntity = vel::Entity{ entity, GetScene() };
		targetTransform = &ccEntity.Transform();
		break;
	}
	selfHealth = &GetComponent<HealthComponent>();

	// Add ParticleSystem
	{
		particlesData.Position = { 0.f, 15.f, 0.f };
		particlesData.VelocityVariation = { 1.f, 2.f, 1.f };
		particlesData.ColorBegin = { 1.f, 1.f, 1.f, 1.f };
		particlesData.ColorEnd = { 0.8f, 0.1f, 0.1f, 1.f };
		particlesData.SizeBegin = 0.1f;
		particlesData.SizeEnd = 0.15f;
		particlesData.LifeTime = 0.5f;
	}

	{
		vel::Entity& globeEntity = GetScene()->TryGetEntityWithTag("Hotel Globe");
		if (globeEntity)
		{
			{
				vel::BoxColliderComponent boxColliderComponent;
				vel::MeshComponent comp;
				if (globeEntity.GetComponentInChild<vel::MeshComponent>(comp))
				{
					boxColliderComponent.aabb = comp.MeshDrawData->aabb;
					boxColliderComponent.aabb.HalfExtent = boxColliderComponent.aabb.HalfExtent * globeEntity.Transform().Scale.x;
					boxColliderComponent.shape = new physics::BoxShape(boxColliderComponent.aabb.HalfExtent);
				}
				globeEntity.AddComponent<vel::BoxColliderComponent>(boxColliderComponent);
			}
			globeEntity.AddComponent<vel::RigidbodyComponent>(vel::RigidbodyComponent());
			vel::RigidbodyComponent* rigidbodyComp = &globeEntity.GetComponent<vel::RigidbodyComponent>();
			vel::BoxColliderComponent& boxCollider = globeEntity.GetComponent<vel::BoxColliderComponent>();
			rigidbodyComp->desc.mass = 0.f;
			rigidbodyComp->desc.isStatic = true;
			rigidbodyComp->desc.IsKinematic = false;
			rigidbodyComp->desc.position = globeEntity.Transform().Translation
				+ glm::vec3(0.f,
					boxCollider.aabb.HalfExtent.y * 2.f,
					0.f);
			rigidbodyComp->rigidBody = GetScene()->GetPhysicsFactory()
				->CreateRigidBody(rigidbodyComp->desc,
					boxCollider.shape);
		}

		// Self
		{
			vel::BoxColliderComponent boxColliderComponent;
			vel::MeshComponent comp;
			if (GetEntity().GetComponentInChild<vel::MeshComponent>(comp))
			{
				boxColliderComponent.aabb = comp.MeshDrawData->aabb;
				boxColliderComponent.aabb.HalfExtent = boxColliderComponent.aabb.HalfExtent * GetEntity().Transform().Scale.x;
				boxColliderComponent.shape = new physics::BoxShape(boxColliderComponent.aabb.HalfExtent);
			}
			GetEntity().AddComponent<vel::BoxColliderComponent>(boxColliderComponent);
		}
		GetEntity().AddComponent<vel::RigidbodyComponent>(vel::RigidbodyComponent());
		vel::RigidbodyComponent* rigidbodyComp = &GetEntity().GetComponent<vel::RigidbodyComponent>();
		vel::BoxColliderComponent& boxCollider = GetEntity().GetComponent<vel::BoxColliderComponent>();
		rigidbodyComp->desc.mass = 300.f;
		rigidbodyComp->desc.isStatic = false;
		rigidbodyComp->desc.IsKinematic = true;
		rigidbodyComp->desc.position = GetEntity().Transform().Translation
			+ glm::vec3(0.f,
				boxCollider.aabb.HalfExtent.y * 2.f,
				0.f);
		rigidbodyComp->rigidBody = GetScene()->GetPhysicsFactory()
			->CreateRigidBody(rigidbodyComp->desc,
				boxCollider.shape);
	}

	rigidBodyComponent = &GetEntity().GetComponent<vel::RigidbodyComponent>();


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
		if (lastSeenHealth > selfHealth->health)
		{
			lastSeenHealth = selfHealth->health;
			for (int i = 0; i < 200; i++)
			{
				particlesData.Position = { selfTransform->Translation.x, 2.5f, selfTransform->Translation.z + 0.5f };
				GetScene()->GetParticleSystem()->Emit(particlesData);
			}
		}
	}
	if (rigidBodyComponent && rigidBodyComponent->rigidBody)
	{
		if (rigidBodyComponent->rigidBody->HasJoints())
		{
			if (!jointsAdded)
			{
				jointsAdded = true;
				jointsFoundTime = std::chrono::steady_clock::now();
			}
			if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - jointsFoundTime).count() > std::chrono::seconds(10).count())
			{
				rigidBodyComponent->rigidBody->ReleaseJoints();
				rigidBodyComponent->desc.IsKinematic = true;
				selfTransform->Translation.y = cachedY;
				jointsAdded = false;
			}
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
		else if (distance > 1.5f && distance < 5.f)
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
