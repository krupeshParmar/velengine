#pragma once
#include "Core.h"
#include "Events/Event.h"
namespace vel {
	class VEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	};

	// Define in app
	Application* CreateApplication();
}
