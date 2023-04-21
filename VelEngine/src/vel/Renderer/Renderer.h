#pragma once
#include "RenderCommand.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "vel/Editor/EditorCamera.h"
#include "Texture.h"
#include "vel/Scene/SceneCamera.h"

namespace vel
{

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void BeginScene();
		static void BeginScene(const SceneCamera& camera,const glm::mat4& transform);
		static void BeginScene(glm::mat4& viewProjectionMatrix);
		static void BeginScene(const glm::mat4& viewMatrix,const glm::mat4& projectionMatrix);
		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);

		//static void Submit(const Ref<VertexArray>& vertexArray);
		static void Submit(const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.f));
		static void Submit(const Ref<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.f));
		static void Submit(const Ref<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray, std::vector<Ref<Texture2D>> textures, const glm::mat4& transform = glm::mat4(1.f));
		static void SubmitInstanced(unsigned int count,const Ref<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray);
		static void Submit(const Ref<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<FrameBuffer> buffer, Ref<FrameBuffer> mainbuffer, const glm::mat4& transform);
		
		static void DrawFullscreenQuad(const Ref<Shader>& shader);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
	};
}