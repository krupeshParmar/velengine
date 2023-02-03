#include "velpch.h"
#include "Core.h"
#include "Application.h"
#include <vel/Log.h>
#include <vel/Events/MouseEvent.h>
#include "Input.h"
#include "vel/Renderer/Renderer.h"

namespace vel {

	Application* Application::s_Instance = nullptr;

	Application::Application()
		:editorCamera(45.f, 1200, 600, 0.1f, 1000.f)
	{
		VEL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(VEL_BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		// Vertex array
		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f,1.f,
			0.0f, 0.5f, 0.0f, 0.77f, 0.77f, 0.2f, 1.f
		};

		// vertex buffer
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position"},
			{ ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);


		uint32_t indices[3] = {
			0, 1, 2
		};

		// index buffer
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
			
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		// shader
		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));


		m_SquareVertexArray.reset(VertexArray::Create());

		float sqVertices[3 * 4] = {
			-0.75,  -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(sqVertices, sizeof(sqVertices)));

		squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position"}
			});
		m_SquareVertexArray->AddVertexBuffer(squareVB);

		uint32_t sq_indices[6] = {
			0, 1, 2, 2, 3, 0
		};

		// index buffer
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(sq_indices, sizeof(sq_indices) / sizeof(uint32_t)));
		m_SquareVertexArray->SetIndexBuffer(squareIB);

		std::string vertexSrc2 = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;
			uniform mat4 u_ViewProjection;
			
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}

		)";
		std::string fragmentSrc2 = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.2, 0.8, 1.0);
			}
		)";

		// shader
		m_Shader2.reset(new Shader(vertexSrc2, fragmentSrc2));

	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VEL_BIND_EVENT_FN(Application::OnWindowClosed));

		//VEL_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{
		while (m_Running)
		{

			RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
			RenderCommand::Clear();

			Renderer::BeginScene();

			m_Shader2->Bind();
			m_Shader2->UploadUniformMat4("u_ViewProjection", editorCamera.GetViewProjection());
			Renderer::Submit(m_SquareVertexArray);

			m_Shader->Bind();
			m_Shader->UploadUniformMat4("u_ViewProjection", editorCamera.GetViewProjection());
			Renderer::Submit(m_VertexArray);

			Renderer::EndScene();
			
			editorCamera.OnUpdate();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
