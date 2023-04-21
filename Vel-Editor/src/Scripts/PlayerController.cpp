#include "PlayerController.h"
#include "HealthComponent.h"


void PlayerController::OnCreate()
{
	FindComponent<vel::AnimatorComponent>(GetScene()->GetEntityWithGUID(GetComponent<vel::IDComponent>().ID), animatorsList);
	FindComponent<vel::RigidbodyComponent>(GetScene()->TryGetEntityWithTag("Mutant"), enemyRigidBodyList);
	characterController = GetComponent<vel::CharacterControllerComponent>().characterController;
	selfTransform = &GetComponent<vel::TransformComponent>();
	selfHealth = &GetComponent<HealthComponent>();
	enemyHealth = &GetScene()->TryGetEntityWithTag("Mutant").GetComponent<HealthComponent>();
	enemyTransform = &GetScene()->TryGetEntityWithTag("Mutant").GetComponent<vel::TransformComponent>();
	globeBody = &GetScene()->TryGetEntityWithTag("Hotel Globe").GetComponent<vel::RigidbodyComponent>();
}
void PlayerController::OnDestroy()
{

}
void PlayerController::OnUpdate(vel::Timestep ts)
{
	if (state == Death)
		return;
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

		if (input)
		{
			{
				moveDir.y = 0.f;
				glm::vec3 normalizeMoveDir = -glm::normalize(moveDir);
				glm::quat lookRotation = glm::quatLookAt(normalizeMoveDir, glm::vec3(0.f, 1.f, 0.f));

				GetComponent<vel::TransformComponent>().SetRotation(
					glm::slerp(lookRotation, GetComponent<vel::TransformComponent>().GetRotation(), ts * ROTATION_SPEED)
				);
			}
			if (speed > WALK_SPEED)
			{
				if (state != Run)
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
				if (state != Walk)
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
		m_Position = characterController->GetPosition();

		if (!attacked && (state == Slash2 || state == Slash) && enemyHealth && enemyTransform)
		{
			if (animatorsList.size() > 0)
			{
				vel::AnimatorComponent* animator = animatorsList[0];
				if (animator->runningAnimation)
				{
					float ratio = animator->animator->GetTimeStamp()
						/ animator->runningAnimation->GetDuration();
					float distance = glm::distance(enemyTransform->Translation, selfTransform->Translation);
					if (ratio > 0.4f && distance < 2.f)
					{
						enemyHealth->health -= 2.f;
						attacked = true;
					}
				}
			}
		}

		if (vel::Input::IsMouseButtonPressed(MouseButton::Left))
		{
			if (state != Slash2 && state != Slash && state != Impact)
			{
				for (vel::AnimatorComponent* animator : animatorsList)
				{
					animator->PlayAnimation(int(Slash2));
				}
				attacked = false;
				state = Slash2;
				input = true;
			}
			else if (state == Slash2 && state != Impact)
			{
				if (animatorsList.size() > 0)
				{
					vel::AnimatorComponent* animator = animatorsList[0];
					float ratio = animator->animator->GetTimeStamp()
						/ animator->runningAnimation->GetDuration();
					if (ratio > 0.6f)
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							animator->PlayAnimation(int(Slash));
						}
						state = Slash;
						attacked = false;
						input = true;
					}
				}
			}
		}
		if (vel::Input::IsKeyPressed(KeyCode::Space) && selfHealth->exp > 30.f && !input)
		{
			if (enemyRigidBodyList.size() > 0)
			{
				vel::RigidbodyComponent* enemy = enemyRigidBodyList[0];
				glm::vec3 enemypos, globePos;

				if (enemy->rigidBody && globeBody && globeBody->rigidBody)
				{
					enemy->rigidBody->GetPosition(enemypos);
					globeBody->rigidBody->GetPosition(globePos);
					physics::DistanceConstraint* distanceConst =
						new physics::DistanceConstraint(
							globeBody->rigidBody,
							enemy->rigidBody,
							enemypos,
							globePos);
					enemy->desc.IsKinematic = false;
					GetScene()->GetPhysicsWorld()->AddConstraint(distanceConst);
					selfHealth->exp -= 30.f;
				}
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

		if (!input)
		{
			if (state != Idle)
			{
				if (state == Slash2 || state == Slash || state == Spell || state == Cast || state == Impact)
				{
					for (vel::AnimatorComponent* animator : animatorsList)
					{
						if (animator->runningAnimation && animator->runningAnimation->Finished)
						{
							animator->PlayAnimation(int(Idle));
							state = Idle;
						}
					}
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

const void PlayerController::TakeDamage(float damage)
{
	if (state == Blocking)
	{
		selfHealth->exp -= damage * 5.f;
		if (selfHealth->exp <= 1.f)
			selfHealth->exp = 0.f;
		else return;
	}
	if (state == Death)
		return;
	state = Impact;

	if (state == Impact)
	{
		if (selfHealth)
		{
			selfHealth->health -= damage;
		
			if (selfHealth->health <= 0.f)
			{
				for (vel::AnimatorComponent* animator : animatorsList)
				{
					if (damage > 10.f)
						animator->PlayAnimation(int(FlyingDeath));
					else animator->PlayAnimation(int(Death));
				}
				state = Death;
				return;
			}
		}
		for (vel::AnimatorComponent* animator : animatorsList)
		{
			animator->PlayAnimation(int(Impact));
		}
		state = Impact;
	}
	damageTaken = damage;
}

const void PlayerController::IncrementXP()
{
	selfHealth->exp += 5.f;
}

const float PlayerController::GetXP()
{
	if (selfHealth)
		return selfHealth->exp;
	return 0.0f;
}

const glm::vec3& PlayerController::GetPosition()
{
	return m_Position;
}
