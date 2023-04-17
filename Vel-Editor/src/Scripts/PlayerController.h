#pragma once
#include "Vel.h"
#include <DistanceConstraint.h>
#include <glm/gtc/quaternion.hpp>

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
	Death
};

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

	virtual void OnCreate() override
	{
		FindComponent<vel::AnimatorComponent>(GetScene()->GetEntityWithGUID(GetComponent<vel::IDComponent>().ID), animatorsList);
		FindComponent<vel::RigidbodyComponent>(GetScene()->TryGetEntityWithTag("Mutant"), enemyRigidBodyList);
		characterController = GetComponent<vel::CharacterControllerComponent>().characterController;
		
	
	}
	virtual void OnDestroy() override
	{

	}
	virtual void OnUpdate(vel::Timestep ts) override
	{
		if (characterController)
		{
			if (vel::Input::IsKeyPressed(KeyCode::LeftShift))
				speed = RUN_SPEED;
			else speed = WALK_SPEED;

			bool input = false;
			glm::vec3 front = glm::vec3(0.f, 0.f, 1.f);
			glm::vec3 right = glm::vec3(1.f, 0.f, 0.f);

			if (cameraController)
			{
				front = cameraController->GetCameraFront();
				right = cameraController->GetCameraRight();
			}
			else
			{
				vel::Entity camEntity = GetScene()->TryGetEntityWithTag("MainCamera");
				if (camEntity)
				{
					ScriptableEntity* script = camEntity.GetComponent<vel::NativeScriptComponent>().Instance;
					cameraController = (CameraController*)script;
				}
			}

			glm::vec3 moveDir = glm::vec3(0.f);
			float horizontalMovement = 0.f;
			float verticalMovement = 0.f;
			if (vel::Input::IsKeyPressed(KeyCode::W))
			{
				input = true;
				//moveDir -= speed * right;
				horizontalMovement = -1.f;
			}

			if (vel::Input::IsKeyPressed(KeyCode::S))
			{
				input = true;
				//moveDir += speed * right;
				horizontalMovement = 1.f;
			}

			if (vel::Input::IsKeyPressed(KeyCode::A))
			{
				input = true;
				//moveDir -= speed * front;
				verticalMovement = -1.f;
			}

			if (vel::Input::IsKeyPressed(KeyCode::D))
			{
				input = true;
				// += speed * front;
				verticalMovement = 1.f;
			}
			glm::vec3 hMove = right * horizontalMovement * speed;
			glm::vec3 vMove = front * verticalMovement * speed;

			moveDir = hMove + vMove;

			//
			//if (vel::Input::IsKeyPressed(KeyCode::W))
			//{
			//	moveDir.z += speed;
			//	input = true;
			//}
			//
			//if (vel::Input::IsKeyPressed(KeyCode::S))
			//{
			//	moveDir.z -= speed;
			//	input = true;
			//}
			//
			//if (vel::Input::IsKeyPressed(KeyCode::A))
			//{
			//	moveDir.x += speed;
			//	input = true;
			//}
			//
			//if (vel::Input::IsKeyPressed(KeyCode::D))
			//{
			//	moveDir.x -= speed;
			//	input = true;
			//}

			if (input)
			{
				{
					moveDir.y = 0.f;
					glm::vec3 normalizeMoveDir = -glm::normalize(moveDir);
					glm::quat lookRotation = glm::quatLookAt(normalizeMoveDir, glm::vec3(0.f, 1.f, 0.f));
					
					GetComponent<vel::TransformComponent>().SetRotation(
						glm::slerp(lookRotation, GetComponent<vel::TransformComponent>().GetRotation(), ts * ROTATION_SPEED)
					);
					/*GetComponent<vel::TransformComponent>().RotationEuler.x = 0.f;
					GetComponent<vel::TransformComponent>().RotationEuler.z = 0.f;*/
				}
				if (speed > WALK_SPEED)
				{
					if (state != Run && state != Slash2)
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							animator->PlayAnimation(int(Run));
						}
						state = Run;
					}
				}
				else
				{
					if (state != Walk && state != Slash2)
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							animator->PlayAnimation(int(Walk));
						}
						state = Walk;
					}
				}
			}

			moveDir.y = -0.981f;	// Gravity
			characterController->Move(moveDir, ts);

			if (vel::Input::IsMouseButtonPressed(MouseButton::Left))
			{
				if (state != Slash2)
				{
					for (vel::AnimatorComponent* animator : animatorsList)
					{
						animator->PlayAnimation(int(Slash2));
					}
					state = Slash2;
					input = true;
				}
			}
			if (vel::Input::IsKeyPressed(KeyCode::Space) && !input)
			{
				if (state != Spell)
				{
					for (vel::AnimatorComponent* animator : animatorsList)
					{
						animator->PlayAnimation(int(Spell));
					}
					if (enemyRigidBodyList.size() > 0)
					{
						vel::RigidbodyComponent* enemy = enemyRigidBodyList[0];
						glm::vec3 enemypos, charPos;

						/*if(enemy->rigidBody)
						{
							enemy->rigidBody->GetPosition(enemypos);
							physics::DistanceConstraint* distanceConst =
								new physics::DistanceConstraint(
									characterController->GetRigidbody(),
									enemy->rigidBody,
									enemypos,
									characterController->GetPosition()
									);
							enemy->desc.IsKinematic = false;
							GetScene()->GetPhysicsWorld()->AddConstraint(distanceConst);
						}*/
					}
					state = Spell;
				}
			}
			if (vel::Input::IsMouseButtonPressed(MouseButton::Right))
			{
				if (state != Blocking)
				{
					for (vel::AnimatorComponent* animator : animatorsList)
					{
						animator->PlayAnimation(int(Blocking));
					}
					state = Blocking;
				}
				input = true;
			}

			if(!input)
			{
				if (state != Idle)
				{
					if (state == Slash2 || state == Slash || state == Spell || state == Cast)
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							if (animator->runningAnimation && animator->runningAnimation->Finished)
							{
								animator->PlayAnimation(int(Idle));
							}
						}
						state = Idle;
					}
					else
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							animator->PlayAnimation(int(Idle));
						}
						state = Idle;
					}
				}
			}
		}
	}

private:
	std::vector<vel::AnimatorComponent*> animatorsList;
	physics::iCharacterController* characterController;
	std::vector < vel::RigidbodyComponent*> enemyRigidBodyList;
	AnimState state = Idle;
	CameraController* cameraController = nullptr;
	const float RUN_SPEED = 5.f;
	const float WALK_SPEED = 2.5f;
	const float ROTATION_SPEED = 0.5f;
	float speed = WALK_SPEED;
};