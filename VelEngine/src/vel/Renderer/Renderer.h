#pragma once
#include "RenderCommand.h"
#include "Shader.h"
#include "vel/Editor/EditorCamera.h"

namespace vel
{

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void BeginScene();
		static void BeginScene(glm::mat4& viewProjectionMatrix);
		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Submit(const Ref<VertexArray>& vertexArray);
		static void Submit(const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.f));
		static void Submit(const Ref<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.f));
		
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
	};
}