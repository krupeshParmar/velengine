#include "velpch.h"
#include "Renderer.h"
#include "Texture.h"
#include <glad/glad.h>

namespace vel
{
	struct RendererData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> FinalTexture;

		uint32_t IndexCount = 0;

		glm::mat4 ViewProjectionMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		float TilingFactor = 1.f;
		vel::Vertices* vertexDataBufferBase = nullptr;
		vel::Vertices* vertexDataBufferptr = nullptr;
	};

	static RendererData s_Data;
	static Ref<VertexArray> m_QuadVertexArray;
	void Renderer::Init()
	{
		VEL_PROFILE_FUNCTION();
		float sqVertices[6 * 4] = {
			0.0f, 0.0f,	-1.f,	-1.f,	0.0f,	1.f,
			1.0f, 0.0f, 1.f,	-1.f,	0.0f,	1.f,
			1.0f, 1.0f, 1.f,	1.f,	0.0f,	1.f,
			0.0f, 1.0f, -1.f,	1.f,	0.0f,	1.f
		};
		m_QuadVertexArray = VertexArray::Create();
		Ref<VertexBuffer> squareVB;
		squareVB = (VertexBuffer::Create(sqVertices, sizeof(sqVertices)));

		squareVB->SetLayout({
				{ ShaderDataType::Float2, "a_TextureCoords"},
				{ ShaderDataType::Float4, "a_Position"}
			});
		m_QuadVertexArray->AddVertexBuffer(squareVB);

		uint32_t sq_indices[6] = {
			0, 1, 2, 2, 3, 0
		};

		// index buffer
		Ref<IndexBuffer> squareIB;
		squareIB = (IndexBuffer::Create(sq_indices, sizeof(sq_indices) / sizeof(uint32_t)));
		m_QuadVertexArray->SetIndexBuffer(squareIB);

		RenderCommand::Init();
	}

	void Renderer::ShutDown()
	{

	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::BeginScene(glm::mat4& viewProjectionMatrix)
	{
		s_Data.ViewProjectionMatrix = viewProjectionMatrix;
		s_Data.vertexDataBufferptr = s_Data.vertexDataBufferBase;
	}

	void Renderer::BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
		s_Data.ProjectionMatrix = projectionMatrix;
		s_Data.ViewMatrix = viewMatrix;
		s_Data.vertexDataBufferptr = s_Data.vertexDataBufferBase;
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	/*void Renderer::Submit(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}*/

	void Renderer::Submit(const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		VEL_PROFILE_FUNCTION();

		s_Data.TilingFactor += 0.001f;
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_View", s_Data.ViewMatrix);
		s_Data.TextureShader->SetMat4("u_Projection", s_Data.ProjectionMatrix);
		s_Data.TextureShader->SetFloat("u_TilingFactor", s_Data.TilingFactor);

		s_Data.TextureShader->SetMat4("u_Transform", transform);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	void Renderer::Submit(const Ref<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		/*shader->SetMat4("u_View", s_Data.ViewMatrix);
		shader->SetMat4("u_Projection", s_Data.ProjectionMatrix);*/
		shader->SetMat4("u_Transform", transform);

		shader->SetFloat("u_TilingFactor", s_Data.TilingFactor);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}
	void Renderer::Submit(const Ref<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::vector<Ref<Texture2D>> textures, const glm::mat4& transform)
	{
		shader->Bind();

		int i = 0;
		for (Ref<Texture2D> texture : textures)
		{
			if(texture && texture->IsLoaded())
				texture->Bind(i);
			i++;
		}

		shader->SetMat4("u_View", s_Data.ViewMatrix);
		shader->SetMat4("u_Projection", s_Data.ProjectionMatrix);
		shader->SetMat4("u_Transform", transform);
		shader->SetMat4("u_InverseTransform", glm::inverse(glm::transpose(transform)));

		shader->SetFloat("u_TilingFactor", s_Data.TilingFactor);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	void Renderer::Submit(const Ref<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<FrameBuffer> buffer, Ref<FrameBuffer> mainbuffer, const glm::mat4& transform)
	{
		//mainbuffer->Unbind();
		//buffer->Bind();
		shader->Bind();
		buffer->BindColorTexture();
		//buffer->Unbind();
		//mainbuffer->Bind();
		shader->Bind();
		shader->SetMat4("u_View", s_Data.ViewMatrix);
		shader->SetMat4("u_Projection", s_Data.ProjectionMatrix);
		shader->SetMat4("u_Transform", transform);
		shader->SetMat4("u_InverseTransform", glm::inverse(glm::transpose(transform)));

		shader->SetFloat("u_TilingFactor", s_Data.TilingFactor);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	void Renderer::DrawFullscreenQuad(const Ref<Shader>& shader)
	{
		shader->Bind();
		shader->SetMat4("u_Transform", glm::mat4(1.f));

		m_QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(m_QuadVertexArray);
		m_QuadVertexArray->Unbind();
	}

}