#include "velpch.h"

#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace vel
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				VEL_CORE_ASSERT(false, "Renderer API None is not supported");
				return nullptr;

			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}