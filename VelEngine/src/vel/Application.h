#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
namespace vel {
	class VEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// Define in app
	Application* CreateApplication();
}
