#include "velpch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace vel
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;

		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;

		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
	Ref<TextureCubeMap> TextureCubeMap::Create(const std::vector<std::string> paths)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;

		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTextureCubeMap>(paths);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}