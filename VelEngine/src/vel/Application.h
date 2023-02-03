#pragma once
#include "Core.h"

#include "Events/Event.h"
#include "vel/LayerStack.h"
#include "Window.h"
#include <vel/Events/ApplicationEvent.h>
#include <vel/ImGui/ImGuiLayer.h>
#include <vel/Renderer/Shader.h>
#include <vel/Renderer/Buffer.h>
#include <vel/Renderer/VertexArray.h>
#include "Editor/EditorCamera.h"

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

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClosed(WindowCloseEvent& e);

		ImGuiLayer* m_ImGuiLayer;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_Shader2;
		std::shared_ptr<VertexArray> m_SquareVertexArray;
		EditorCamera editorCamera;
	private:
		static Application* s_Instance;
	};

	// Define in app
	Application* CreateApplication();
}
