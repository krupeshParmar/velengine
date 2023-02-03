#include <Vel.h>

class ExampleLayer : public vel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), editorCamera(45.f, 800, 600, 0.1f, 1000.f), m_CameraPositon(0.f)
	{
		// Vertex array
		m_VertexArray.reset(vel::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f,1.f,
			0.0f, 0.5f, 0.0f, 0.77f, 0.77f, 0.2f, 1.f
		};

		// vertex buffer
		std::shared_ptr<vel::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(vel::VertexBuffer::Create(vertices, sizeof(vertices)));

		vel::BufferLayout layout = {
			{ vel::ShaderDataType::Float3, "a_Position"},
			{ vel::ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);


		uint32_t indices[3] = {
			0, 1, 2
		};

		// index buffer
		std::shared_ptr<vel::IndexBuffer> indexBuffer;
		indexBuffer.reset(vel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
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
		m_Shader.reset(new vel::Shader(vertexSrc, fragmentSrc));


		m_SquareVertexArray.reset(vel::VertexArray::Create());

		float sqVertices[3 * 4] = {
			-0.75,  -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<vel::VertexBuffer> squareVB;
		squareVB.reset(vel::VertexBuffer::Create(sqVertices, sizeof(sqVertices)));

		squareVB->SetLayout({
				{ vel::ShaderDataType::Float3, "a_Position"}
			});
		m_SquareVertexArray->AddVertexBuffer(squareVB);

		uint32_t sq_indices[6] = {
			0, 1, 2, 2, 3, 0
		};

		// index buffer
		std::shared_ptr<vel::IndexBuffer> squareIB;
		squareIB.reset(vel::IndexBuffer::Create(sq_indices, sizeof(sq_indices) / sizeof(uint32_t)));
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
		m_Shader2.reset(new vel::Shader(vertexSrc2, fragmentSrc2));
	}

	void OnUpdate() override
	{
		vel::RenderCommand::SetClearColor(glm::vec4(0.9f, 0.1f, 0.9f, 1));
		vel::RenderCommand::Clear();

		vel::Renderer::BeginScene();

		m_Shader2->Bind();
		m_Shader2->UploadUniformMat4("u_ViewProjection", editorCamera.GetViewProjection());
		vel::Renderer::Submit(m_SquareVertexArray);

		m_Shader->Bind();
		m_Shader->UploadUniformMat4("u_ViewProjection", editorCamera.GetViewProjection());
		vel::Renderer::Submit(m_VertexArray);

		vel::Renderer::EndScene();

		editorCamera.OnUpdate();
	}

	void OnImGuiRender() override
	{
		
	}

	void OnEvent(vel::Event& event) override
	{
		editorCamera.OnEvent(event);
		vel::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<vel::MouseScrolledEvent>(VEL_BIND_EVENT_FN(ExampleLayer::OnMouseScrolledEvent));
	}

	bool OnMouseScrolledEvent(vel::MouseScrolledEvent& event)
	{
		//editorCamera.OnEvent(event);
		return false;
	}

private:
	std::shared_ptr<vel::Shader> m_Shader;
	std::shared_ptr<vel::VertexArray> m_VertexArray;

	std::shared_ptr<vel::Shader> m_Shader2;
	std::shared_ptr<vel::VertexArray> m_SquareVertexArray;
	vel::EditorCamera editorCamera;
	glm::vec3 m_CameraPositon;
};

class VelApp : public vel::Application
{
public:
	VelApp()
	{
		PushLayer(new ExampleLayer());
	}

	~VelApp()
	{

	}
};

vel::Application* vel::CreateApplication()
{
	return new VelApp();
}