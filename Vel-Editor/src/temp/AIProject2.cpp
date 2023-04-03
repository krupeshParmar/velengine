#include "AIProject2.h"
#include <imgui/imgui.h>

AIProject2::AIProject2()
{
	{
		m_ShaderLibrary.Load("assets/shaders/simpleshader1.glsl");
		vel::FrameBufferSpecification fbSpec;
		fbSpec.Width = vel::Application::Get().GetWindow().GetWidth();
		fbSpec.Height = vel::Application::Get().GetWindow().GetHeight();
		fullScreenFrameBuffer = vel::FrameBuffer::Create(fbSpec);
		m_SquareVertexArray = vel::VertexArray::Create();
		float sqVertices[6 * 4] = {
			0.0f, 0.0f,	-0.75,	-0.75f,	0.0f,	1.f,
			1.0f, 0.0f, 0.75f,	-0.75f,	0.0f,	1.f,
			1.0f, 1.0f, 0.75f,	0.75f,	0.0f,	1.f,
			0.0f, 1.0f, -0.75f,	0.75f,	0.0f,	1.f
		};
		m_SquareVertexArray = vel::VertexArray::Create();
		vel::Ref<vel::VertexBuffer> squareVB;
		squareVB = (vel::VertexBuffer::Create(sqVertices, sizeof(sqVertices)));

		squareVB->SetLayout({
				{ vel::ShaderDataType::Float2, "a_TextureCoords"},
				{ vel::ShaderDataType::Float4, "a_Position"}
			});
		m_SquareVertexArray->AddVertexBuffer(squareVB);

		uint32_t sq_indices[6] = {
			0, 1, 2, 2, 3, 0
		};

		// index buffer
		vel::Ref<vel::IndexBuffer> squareIB;
		squareIB = (vel::IndexBuffer::Create(sq_indices, sizeof(sq_indices) / sizeof(uint32_t)));
		m_SquareVertexArray->SetIndexBuffer(squareIB);
	}
}

AIProject2::~AIProject2()
{
}

void AIProject2::OnUpdate(vel::Timestep ts)
{
	fullScreenFrameBuffer->Bind();
	vel::RenderCommand::SetClearColor(glm::vec4(0.3f, 0.3f, 0.3f, 1));
	vel::RenderCommand::Clear();
	vel::RenderCommand::SetCullFace();
	float ratio = (float)vel::Application::Get().GetWindow().GetWidth() / vel::Application::Get().GetWindow().GetHeight();

	m_ShaderLibrary.Get("simpleshader1")->SetBool("useTextureDiffuse", false);
	m_ShaderLibrary.Get("simpleshader1")->SetFloat4("RGBA", { 1.f, 1.f, 0.f, 1.f });

	vel::Renderer::Submit(m_ShaderLibrary.Get("simpleshader1"), m_SquareVertexArray, glm::mat4(1.f));

	fullScreenFrameBuffer->Unbind();
}

void AIProject2::OnImGuiRender()
{
	ImGui::Begin("Viewport");

	uint32_t textureID = fullScreenFrameBuffer->GetColorAttachmenRendererID();
	ImGui::Image(&textureID, { 1024, 1024 });

	ImGui::End();
}

void AIProject2::OnEvent(vel::Event& event)
{
}
