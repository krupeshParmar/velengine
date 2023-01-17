#pragma once
#include "Core.h"

#include "Events/Event.h"
#include "vel/LayerStack.h"
#include "Window.h"
#include <vel/Events/ApplicationEvent.h>
namespace vel {
	class VEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClosed(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	// Define in app
	Application* CreateApplication();
}
