#include "velpch.h"

#include "VertexArray.h"
#include "Renderer.h"
#include "vel/Platform/OpenGL/OpenGLVertexArray.h"

namespace vel
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
				VEL_CORE_ASSERT(false, "Renderer API None is not supported");
				return nullptr;

			case RendererAPI::OpenGL: return new OpenGLVertexArray();
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}