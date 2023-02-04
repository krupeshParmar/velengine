#pragma once
#include "RenderCommand.h"
#include "Shader.h"
#include "vel/Editor/EditorCamera.h"

namespace vel
{

	class Renderer
	{
	public:
		static void BeginScene();
		static void BeginScene(EditorCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
		static void Submit(const std::shared_ptr<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.f));
		
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}