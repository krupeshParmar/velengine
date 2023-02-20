#include "velpch.h"
#include "Core.h"
#include "Application.h"
#include <vel/Log.h>
#include <vel/Events/MouseEvent.h>
#include "Input.h"
#include "vel/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace vel {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		VEL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(VEL_BIND_EVENT_FN(Application::OnEvent));

		//Renderer::Init();

		//m_Window->SetVSync(false);
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VEL_BIND_EVENT_FN(Application::OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(VEL_BIND_EVENT_FN(Application::OnWindowResize));

		//VEL_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{
		RenderCommand::SetViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
		while (m_Running)
		{
			float time = (float) glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}
