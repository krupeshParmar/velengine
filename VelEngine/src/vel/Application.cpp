#include "velpch.h"
#include "Application.h"
#include <vel/Events/ApplicationEvent.h>
#include <vel/Log.h>
#include <vel/Events/MouseEvent.h>
#include <GLFW/glfw3.h>

namespace vel {
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}
	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0, 1, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}
}
