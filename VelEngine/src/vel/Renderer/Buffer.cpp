#include "velpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace vel
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
				VEL_CORE_ASSERT(false, "Renderer API None is not supported");
				return nullptr;
			case RendererAPI::API::OpenGL:

				return new OpenGLVertexBuffer(vertices, size);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:

			return new OpenGLIndexBuffer(indices, size);
		default:
			VEL_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
		}
	}
}