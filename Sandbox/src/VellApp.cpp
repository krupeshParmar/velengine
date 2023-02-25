#include <Vel.h>
#include <vel/Core/EntryPoint.h>

#include <glm/gtx/transform.hpp>
#include <iPhysicsFactory.h>
#include <PhysicsFactory.h>
#include "temp/PhysicsProject1.h"
#include "Example.h"
#include "EditorLayer.h"

class VelApp : public vel::Application
{
public:
	VelApp()
	{
		//PushLayer(new ExampleLayer());
		//PushLayer(new ExampleLayer2());
		PushLayer(new PhysicsScene());
		//PushLayer(new EditorLayer());
	}

	~VelApp()
	{

	}
};

vel::Application* vel::CreateApplication()
{
	return new VelApp();
}
