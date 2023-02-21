#define MAIN_SHOULD_BE_HERE
#include <Vel.h>
#include <glm/gtx/transform.hpp>
#include <iPhysicsFactory.h>
#include <PhysicsFactory.h>
#include "temp/PhysicsProject1.h"
#include "Example.h"

class VelApp : public vel::Application
{
public:
	VelApp()
	{
		//PushLayer(new ExampleLayer());
		//PushLayer(new ExampleLayer2());
		PushLayer(new PhysicsScene());
	}

	~VelApp()
	{

	}
};

vel::Application* vel::CreateApplication()
{
	return new VelApp();
}
