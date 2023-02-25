#include "velpch.h"
#include "Renderer.h"
#include "Texture.h"

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
		float TilingFactor = 1.f;
		vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones* vertexDataBufferBase = nullptr;
		vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones* vertexDataBufferptr = nullptr;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		VEL_PROFILE_FUNCTION();
		s_Data.QuadVertexArray = VertexArray::Create();
		Ref<VertexBuffer> squareVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones));
		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TextureCoords"},
			});
		s_Data.QuadVertexArray->AddVertexBuffer(squareVB);

		s_Data.vertexDataBufferBase = new vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices,s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_Data.FinalTexture = Texture2D::Create(1, 1);
		uint32_t textureData = 0xffffffff;
		s_Data.FinalTexture->SetData(&textureData, sizeof(uint32_t));

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetInt("u_Texture", 0);
		s_Data.TextureShader->SetFloat("u_TilingFactor", s_Data.TilingFactor);


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

	void Renderer::EndScene()
	{
		
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		VEL_PROFILE_FUNCTION();


		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
		s_Data.TextureShader->SetFloat("u_TilingFactor", s_Data.TilingFactor);
		//s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_Transform", transform);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	void Renderer::Submit(const Ref<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

}