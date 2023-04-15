#pragma once
#include "Vel.h"

class CameraController : public vel::ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
		//GetComponent<TransformComponent>();
		VEL_CORE_INFO("Camera Controller Created");
		lastMouseX = vel::Input::GetMouseX();
		lastMouseY = vel::Input::GetMouseY();
		auto cameraView = GetScene()->GetAllEntitiesWith<vel::CameraComponent>();
		for (entt::entity entity : cameraView)
		{
			vel::Entity cameraEntity = vel::Entity{ entity, GetScene() };
			camera = &cameraEntity.GetComponent<vel::CameraComponent>();
			if (camera->Primary)
			{
				self = &cameraEntity.Transform();
				break;
			}
		}
		auto ccView = GetScene()->GetAllEntitiesWith<vel::CharacterControllerComponent>();
		for (entt::entity entity : ccView)
		{
			vel::Entity ccEntity = vel::Entity{ entity, GetScene() };
			targetTransform = &ccEntity.Transform();
			break;
		}
		vel::Application::Get().DisableMouse();
	}

	virtual void OnDestroy() override
	{

	}

	virtual void OnUpdate(vel::Timestep ts) override
	{
		if (vel::Input::IsKeyPressed(KeyCode::Escape))
		{
			if (showMouse)
			{
				showMouse = false;
				vel::Application::Get().DisableMouse();
			}
			else{
				showMouse = true;
				vel::Application::Get().EnableMouse();
			}
		}
		CalculatePitch(ts);
		CalculateAngleAroundPlayer(ts);
		float horizontalDistance = CalculateHorizontalDistance();
		float verticalDistance = CalculateVerticalDistance();
		if (targetTransform && self)
		{
			CalculateCameraPosition(horizontalDistance, verticalDistance);
			yaw = 180.f - (targetTransform->RotationEuler.y + angleAroundPlayer);
			self->Translation = targetTransform->Translation + glm::vec3(0.f, yOffset, -distance);
			//camera->Camera.Position = self->Translation;
			camera->Camera.Target = targetTransform->Translation + glm::vec3(0.f, yOffset, 0.f);
		}
	}

private:
	void CalculatePitch(vel::Timestep ts)
	{
		if (lastMouseY != vel::Input::GetMouseY())
		{
			float pitchChange = (vel::Input::GetMouseY() - lastMouseY) * ts * speedY;
			pitch += pitchChange;
			lastMouseY = vel::Input::GetMouseY();
		}
	}

	void CalculateAngleAroundPlayer(vel::Timestep ts)
	{
		if (lastMouseX != vel::Input::GetMouseX())
		{
			float angleChange = (vel::Input::GetMouseX() - lastMouseX) * ts * speedX;
			angleAroundPlayer -= angleChange;
			lastMouseX = vel::Input::GetMouseX();
		}
	}

	float CalculateHorizontalDistance()
	{
		return (float) distance * cos(glm::radians(pitch));
	}

	float CalculateVerticalDistance()
	{
		return (float) distance * sin(glm::radians(pitch));
	}

	void CalculateCameraPosition(float hor, float ver)
	{
		float theta = targetTransform->RotationEuler.y + angleAroundPlayer;
		float offsetX = (float) hor * sin(glm::radians(theta));
		float offsetZ = (float) hor * cos(glm::radians(theta));
		glm::vec3 position;
		position.x = targetTransform->Translation.x - offsetX;
		position.y = targetTransform->Translation.y + ver;
		position.z = targetTransform->Translation.z - offsetZ;
		camera->Camera.Position = position;
	}

private:
	float rotationX;
	float rotationY;
	float distance = 5.f;
	float yOffset = 1.5f;
	float angleAroundPlayer = 0.f;
	float pitch = 20.f;
	float yaw = 0.f;
	float roll;
	float lastMouseX = 0.f;
	float lastMouseY = 0.f;
	vel::TransformComponent* targetTransform = nullptr;
	vel::TransformComponent* self = nullptr;
	vel::CameraComponent* camera = nullptr;
	float speedX = 3.f;
	float speedY = 3.f;
	bool showMouse = false;
};