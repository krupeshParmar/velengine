#include "Application.h"
#include <vel/Events/ApplicationEvent.h>
#include <vel/Log.h>
#include <vel/Events/MouseEvent.h>

namespace vel {
	Application::Application()
	{
	}

	Application::~Application()
	{
	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryInput))
		{
			VEL_TRACE(e);
		}
		else {
			VEL_ERROR("Not an Input Event");
		}
		while (true);
	}
}
