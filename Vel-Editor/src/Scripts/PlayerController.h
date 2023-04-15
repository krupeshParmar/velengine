#pragma once
#include "Vel.h"

enum AnimState
{
	Idle,
	Slash,
	Walk,
	Run,
	Jump,
	SuperSlash
};

class PlayerController : public vel::ScriptableEntity
{
public:

	void FindAnimators(vel::Entity entity)
	{
		if (entity.HasComponent<vel::AnimatorComponent>())
		{
			animatorsList.push_back(&entity.GetComponent<vel::AnimatorComponent>());
		}
		if (entity.Children().size() > 0)
		{
			for (vel::GUID guid : entity.Children())
			{
				FindAnimators(GetScene()->GetEntityWithGUID(guid));
			}
		}
	}

	virtual void OnCreate() override
	{
		FindAnimators(GetScene()->GetEntityWithGUID(GetComponent<vel::IDComponent>().ID));
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
				speed = 3.f;
			else speed = 1.5f;

			bool input = false;

			glm::vec3 moveDir = glm::vec3(0.f);
			moveDir.y = -0.981f;	// Gravity
			if (vel::Input::IsKeyPressed(KeyCode::Space))
			{
				moveDir.y += 5.f;
			}
			if (vel::Input::IsKeyPressed(KeyCode::W))
			{
				moveDir.z += speed;
				input = true;
			}

			if (vel::Input::IsKeyPressed(KeyCode::S))
			{
				moveDir.z -= speed;
				input = true;
			}

			if (vel::Input::IsKeyPressed(KeyCode::A))
			{
				moveDir.x += speed;
				input = true;
			}

			if (vel::Input::IsKeyPressed(KeyCode::D))
			{
				moveDir.x -= speed;
				input = true;
			}

			if (input)
			{
				if (speed > 1.6f)
				{
					if (state != Run && state != Slash)
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							animator->PlayAnimation(4);
						}
						state = Run;
					}
				}
				else
				{
					if (state != Walk && state != Slash)
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							animator->PlayAnimation(3);
						}
						state = Walk;
					}
				}
			}

			characterController->Move(moveDir, ts);
			if (vel::Input::IsMouseButtonPressed(MouseButton::Left))
			{
				if (state != Slash)
				{
					for (vel::AnimatorComponent* animator : animatorsList)
					{
						animator->PlayAnimation(2);
					}
					state = Slash;
					input = true;
				}
			}

			if(!input)
			{
				if (state != Idle)
				{
					if (state == Slash)
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							if (animator->runningAnimation && animator->runningAnimation->Finished)
							{
								animator->PlayAnimation(0);
							}
						}
						state = Idle;
					}
					else
					{
						for (vel::AnimatorComponent* animator : animatorsList)
						{
							animator->PlayAnimation(0);
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
	AnimState state = Idle;
	float speed = 1.5f;
};