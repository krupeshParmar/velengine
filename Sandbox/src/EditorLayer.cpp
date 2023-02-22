#include "EditorLayer.h"

#include <chrono>

class Timer
{
public:
	Timer(const char* name)
		:m_Name(name), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}
	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}
	void Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		m_Stopped = true;
		float duration = (end - start) * 0.001f;
		std::cout << "Duration: " << duration << "ms" << std::endl;
	}


private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

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
	Timer timer("EditorLayer::OnUpdate");
	m_EditorCamera.OnUpdate(ts);

	vel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
	vel::RenderCommand::Clear();


	vel::Renderer::BeginScene(m_EditorCamera.GetViewProjection());

	m_Texture->Bind(0);
	vel::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));
	
	m_SecondTexture->Bind(0);
	vel::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));

	vel::Renderer::EndScene();
}

void EditorLayer::OnImGuiRender()
{
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
