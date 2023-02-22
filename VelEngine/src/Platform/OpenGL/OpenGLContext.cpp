#include "velpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <vel/Core/Log.h>

namespace vel
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		VEL_CORE_ASSERT(windowHandle, "Window handle is null!");
	}
	void OpenGLContext::Init()
	{
		VEL_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VEL_CORE_ASSERT(status, "Failed to initiaize glad!");
		std::stringstream ssvendor, ssrenderer, ssversion;
		std::string vendor, renderer, version;

		ssvendor << glGetString(GL_VENDOR);
		vendor = ssvendor.str();

		ssrenderer << glGetString(GL_RENDERER);
		renderer = ssrenderer.str();

		ssversion << glGetString(GL_VERSION);
		version = ssversion.str();

		VEL_CORE_INFO("OpenGLInfo: ");
		VEL_CORE_INFO("  Vendor: {0}", vendor);
		VEL_CORE_INFO("  Renderer: {0}", renderer);
		VEL_CORE_INFO("  Version: {0}", version);

	}
	void OpenGLContext::SwapBuffers()
	{
		VEL_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}
