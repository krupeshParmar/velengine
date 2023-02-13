#include "velpch.h"

#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace vel
{
	Shader* Shader::Create(const std::string& shaderFilePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;

		case RendererAPI::API::OpenGL: return new OpenGLShader(shaderFilePath);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VEL_CORE_ASSERT(false, "Renderer API None is not supported");
			return nullptr;

		case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}
		VEL_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}