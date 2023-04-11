#include <Vel.h>
#include <vel/Core/EntryPoint.h>

#include <glm/gtx/transform.hpp>
#include <iPhysicsFactory.h>
#include <PhysicsFactory.h>
#include "Example.h"
#include "EditorLayer.h"
#include "EditorLayer2.h"

namespace vel
{
	class VelEditor : public Application
	{
	public:
		VelEditor()
		{
			//PushLayer(new ExampleLayer());
			//PushLayer(new ExampleLayer2());
			//PushLayer(new EditorLayer());
			PushLayer(new EditorLayer2());
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
