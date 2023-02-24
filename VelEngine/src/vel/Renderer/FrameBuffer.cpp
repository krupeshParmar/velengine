#include "velpch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace vel
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:

			return CreateRef<OpenGLFrameBuffer>(spec);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}