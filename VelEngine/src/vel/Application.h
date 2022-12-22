#pragma once
#include "Core.h"
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
