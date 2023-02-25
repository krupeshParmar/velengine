#include <velpch.h>
#include "vel/Core/Input.h"
#include <GLFW/glfw3.h>
#include <vel/Core/Application.h>

namespace vel
{
	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, (int) keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool Input::IsMouseButtonPressed(MouseButton button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, (int)button);
		return state == GLFW_PRESS;
	}
	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { xpos, ypos };
	}
	float Input::GetMouseX()
	{
		auto[x,y] = GetMousePosition();
		return x;
	}
	float Input::GetMouseY()
	{
		auto[x, y] = GetMousePosition();
		return y;
	}
}
