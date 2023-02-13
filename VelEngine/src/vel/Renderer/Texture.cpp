#include "velpch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace vel
{
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;

		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}