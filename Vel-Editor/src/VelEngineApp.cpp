#include <Vel.h>
#include <vel/Core/EntryPoint.h>

#include <glm/gtx/transform.hpp>
#include <iPhysicsFactory.h>
#include <PhysicsFactory.h>
#include "temp/PhysicsProject1.h"
#include "Example.h"
#include "EditorLayer.h"

namespace vel
{
	class VelEditor : public Application
	{
	public:
		VelEditor()
		{
			//PushLayer(new ExampleLayer());
			//PushLayer(new ExampleLayer2());
			//PushLayer(new PhysicsScene());
			PushLayer(new EditorLayer());
		}

		~VelEditor()
		{

		}
	};

	vel::Application* CreateApplication()
	{
		return new VelEditor();
	}
}
