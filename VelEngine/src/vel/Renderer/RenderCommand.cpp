#include "velpch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace vel
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}