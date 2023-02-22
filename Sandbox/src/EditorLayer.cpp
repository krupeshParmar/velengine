#include "EditorLayer.h"

#include <chrono>
#include <imgui/imgui.h>

EditorLayer::EditorLayer()
	: Layer("EditorLayer"), m_EditorCamera(45.f, 1280, 720, 0.1f, 1000.f)
{
	
}

void EditorLayer::OnAttach()
{
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
	m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
	m_Texture = vel::Texture2D::Create(1, 1);
	uint32_t whiteTexture = 0xffffff77;
	m_Texture->SetData(&whiteTexture, sizeof(uint32_t));
	m_SecondTexture = vel::Texture2D::Create("assets/textures/jolyne2.png");

	std::dynamic_pointer_cast<vel::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->Bind();
	std::dynamic_pointer_cast<vel::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->UploadUniformInt("u_Texture", 0);
	vel::RenderCommand::Init();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(vel::Timestep ts)
{
	VEL_PROFILE_FUNCTION();
	{
		VEL_PROFILE_SCOPE("EditorCamera::OnUpdate");
		m_EditorCamera.OnUpdate(ts);
	}

	{
		VEL_PROFILE_SCOPE("Renderer Prep");
		vel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		vel::RenderCommand::Clear();
	}

	{
		VEL_PROFILE_SCOPE("Renderer Draw");
		vel::Renderer::BeginScene(m_EditorCamera.GetViewProjection());

		m_Texture->Bind(0);
		vel::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));

		m_SecondTexture->Bind(0);
		vel::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));

		vel::Renderer::EndScene();
	}
}

void EditorLayer::OnImGuiRender()
{
	VEL_PROFILE_FUNCTION();
	ImGui::Begin("Profile");
	ImGui::End();
}

void EditorLayer::OnEvent(vel::Event& event)
{
	m_EditorCamera.OnEvent(event);
	if (event.GetEventType() == vel::EventType::WindowResize)
	{
		auto& we = (vel::WindowResizeEvent&)event;
		m_EditorCamera.SetViewportSize(we.GetWidth(), we.GetHeight());
	}
}
