#pragma once
#include "Vel.h"

class CameraController : public vel::ScriptableEntity
{
public:
	virtual void OnCreate() override
	{
		//GetComponent<TransformComponent>();
		VEL_CORE_INFO("Camera Controller Created");
	}

	virtual void OnDestroy() override
	{

	}

	virtual void OnUpdate(vel::Timestep ts) override
	{
		//VEL_CORE_TRACE("Timestep: {0}", ts.GetSeconds());
		float speed = 5.f;
		vel::CameraComponent& camera = GetComponent<vel::CameraComponent>();
		if (vel::Input::IsKeyPressed(KeyCode::A))
		{
			camera.Camera.Position.x -= speed * ts;
		}
		if (vel::Input::IsKeyPressed(KeyCode::D))
		{
			camera.Camera.Position.x += speed * ts;
		}
		if (vel::Input::IsKeyPressed(KeyCode::W))
		{
			camera.Camera.Position.z += speed * ts;
		}
		if (vel::Input::IsKeyPressed(KeyCode::S))
		{
			camera.Camera.Position.z -= speed * ts;
		}
		if (vel::Input::IsKeyPressed(KeyCode::Q))
		{
			camera.Camera.Position.y -= speed * ts;
		}
		if (vel::Input::IsKeyPressed(KeyCode::E))
		{
			camera.Camera.Position.y += speed * ts;
		}
	}
};