#pragma once
#include "Core.h"

#include "../Events/Event.h"
#include "vel/Core/LayerStack.h"
#include "Window.h"
#include <vel/Events/ApplicationEvent.h>
#include <vel/ImGui/ImGuiLayer.h>
#include "vel/Core/Timestep.h"

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

		void Close();

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ImGuiLayer* m_ImGuiLayer;
		Scope<Window> m_Window;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.f;

		
	private:
		static Application* s_Instance;
	};

	// Define in app
	Application* CreateApplication();
}
