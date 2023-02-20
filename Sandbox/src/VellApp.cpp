#define MAIN_SHOULD_BE_HERE
#include <Vel.h>
#include <glm/gtx/transform.hpp>
#include <iPhysicsFactory.h>
#include <PhysicsFactory.h>
#include "temp/PhysicsProject1.h"

class ExampleLayer : public vel::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), editorCamera(45.f, 1280, 720, 0.1f, 1000.f)
	{
		// Vertex array
		m_SquareVertexArray = vel::VertexArray::Create();
		vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones* pvertices = new vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones[4];
		float sqVertices[3 * 4] = {
			-0.75,  -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		for (int i = 0; i < 4; i++)
		{
			pvertices[i].r = 0.8f;
			pvertices[i].g = 0.1f;
			pvertices[i].b = 0.7f;
			pvertices[i].a = 1.f;
			pvertices[i].w = 1.f;
		}
		pvertices[0].x = -0.75;
		pvertices[0].y = -0.75f;
		pvertices[0].z = 0.f;

		pvertices[1].x = 0.75f;
		pvertices[1].y = -0.75f;
		pvertices[1].z = 0.f;

		pvertices[2].x = 0.75f;
		pvertices[2].y = 0.75f;
		pvertices[2].z = 0.f;

		pvertices[3].x = -0.75f;
		pvertices[3].y = 0.75f;
		pvertices[3].z = 0.f;
		vel::Ref<vel::VertexBuffer> squareVB;
		squareVB = (vel::VertexBuffer::Create(pvertices, sizeof(vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones) * 4));

		squareVB->SetLayout({
			{ vel::ShaderDataType::Float4, "vColor"},
			{ vel::ShaderDataType::Float4, "aPosition"},
			{ vel::ShaderDataType::Float4, "vNormal"},
			{ vel::ShaderDataType::Float4, "vUVx2"},
			{ vel::ShaderDataType::Float4, "vTangent"},
			{ vel::ShaderDataType::Float4, "vBiNormal"},
			{ vel::ShaderDataType::Float4, "vBoneID"},
			{ vel::ShaderDataType::Float4, "vBoneWeight"}
			});
		m_SquareVertexArray->AddVertexBuffer(squareVB);

		uint32_t sq_indices[6] = {
			0, 1, 2, 2, 3, 0
		};

		// index buffer
		vel::Ref<vel::IndexBuffer> squareIB;
		int count = sizeof(sq_indices) / sizeof(uint32_t);
		squareIB = (vel::IndexBuffer::Create(sq_indices, count));
		m_SquareVertexArray->SetIndexBuffer(squareIB);

		/*
		*
		* 	struct sVertex_RGBA_XYZ_N_UV_T_BiN_Bones
		*	{
		*		float r, g, b, a;		//	in vec4 vColour;			// Was vec3
		*		float x, y, z, w;		//	in vec4 vPosition;			// Was vec3
		*		float nx, ny, nz, nw;	//	in vec4 vNormal;			// Vertex normal
		*		float u0, v0, u1, v1;	//	in vec4 vUVx2;				// 2 x Texture coords
		*		float tx, ty, yz, tw;	//	in vec4 vTangent;			// For bump mapping
		*		float bx, by, bz, bw;	//	in vec4 vBiNormal;			// For bump mapping
		*		float vBoneID[4];		//	in vec4 vBoneID;		// For skinned mesh (FBX)
		*		float vBoneWeight[4];	//	in vec4 vBoneWeight;		// For skinned mesh (FBX)
		*	};
		*/
		std::string vertexSrc2 = R"(
				#version 330 core
				layout(location = 0)in vec4 vColour;			// Was vec3
				layout(location = 1)in vec4 aPosition;			// Was vec3
				layout(location = 2)in vec4 vNormal;			// Vertex normal
				layout(location = 3)in vec4 vUVx2;				// 2 x Texture coords
				layout(location = 4)in vec4 vTangent;			// For bump mapping
				layout(location = 5)in vec4 vBiNormal;			// For bump mapping
				layout(location = 6)in vec4 vBoneID;			// For skinned mesh (FBX)
				layout(location = 7)in vec4 vBoneWeight;		// For skinned mesh (FBX)
				out vec4 v_Position;
				out vec4 v_Color;
				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;
			
				void main()
				{
					v_Position = aPosition;
					v_Color = vColour;
					gl_Position = u_ViewProjection * u_Transform * aPosition;
				}
			)";
		std::string fragmentSrc2 = R"(
				#version 330 core
				layout(location = 0) out vec4 color;
				in vec4 v_Position;
				in vec4 v_Color;
				void main()
				{
					color = v_Color;
				}
			)";

		// shader
		m_Shader2 = vel::Shader::Create("shader2",vertexSrc2, fragmentSrc2);
	}

	void OnUpdate(vel::Timestep ts) override
	{
		editorCamera.OnUpdate(ts);
		float ratio;
		int width, height;
		width = vel::Application::Get().GetWindow().GetWidth();
		height = vel::Application::Get().GetWindow().GetHeight();
		ratio = width / (float)height;
		vel::RenderCommand::SetViewport(0, 0, width, height);
		vel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		vel::RenderCommand::Clear();


		vel::Renderer::BeginScene(editorCamera.GetViewProjection());

		vel::Renderer::Submit(m_Shader2, m_SquareVertexArray);

		vel::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{

	}

	void OnEvent(vel::Event& event) override
	{
		VEL_TRACE("{0}", event);
		editorCamera.OnEvent(event);
		vel::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<vel::MouseScrolledEvent>(VEL_BIND_EVENT_FN(ExampleLayer::OnMouseScrolledEvent));
	}

	bool OnMouseScrolledEvent(vel::MouseScrolledEvent& event)
	{
		editorCamera.OnEvent(event);
		return false;
	}

	private:
		physics::iPhysicsFactory* physicsFactory;
		vel::Ref<vel::Shader> m_Shader2;
		vel::Ref<vel::VertexArray> m_SquareVertexArray;
		vel::EditorCamera editorCamera;
	};
class ExampleLayer2 : public vel::Layer
{
public:
	ExampleLayer2()
		: Layer("Example"), editorCamera(45.f, 1280, 720, 0.1f, 1000.f)
	{
		// Vertex array
		m_SquareVertexArray = vel::VertexArray::Create();
		float sqVertices[6 * 4] = {
			0.0f, 0.0f,	-0.75,	-0.75f,	0.0f,	1.f,
			1.0f, 0.0f, 0.75f,	-0.75f,	0.0f,	1.f,
			1.0f, 1.0f, 0.75f,	0.75f,	0.0f,	1.f,
			0.0f, 1.0f, -0.75f,	0.75f,	0.0f,	1.f
		};
		m_SquareVertexArray = vel::VertexArray::Create();

		/*float sqVertices[3 * 4] = {
			-0.75,  -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};*/

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
		std::string vertexSrc2 = R"(
			#version 330 core
			layout(location = 0) in vec2 a_TextureCoords;
			layout(location = 1) in vec4 a_Position;
			out vec4 v_Position;
			out vec4 v_Colour;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			
			void main()
			{
				v_Position = a_Position;
				v_Colour = vec4(1.0, 1.0, 0.0, 1.0);
				gl_Position = u_ViewProjection * u_Transform * a_Position;
			}
		)";
		std::string fragmentSrc2 = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec4 v_Position;
			in vec4 v_Colour;
			void main()
			{
				color = v_Colour;
			}
		)";

		// shader
		m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
		m_Shader2 = vel::Shader::Create("shader2",vertexSrc2, fragmentSrc2);
		m_Texture = vel::Texture2D::Create("assets/textures/CheckerBoard.png");
		m_SecondTexture = vel::Texture2D::Create("assets/textures/jolyne2.png");

		std::dynamic_pointer_cast<vel::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->Bind();
		std::dynamic_pointer_cast<vel::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(vel::Timestep ts) override
	{
		editorCamera.OnUpdate(ts);

		vel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		vel::RenderCommand::Clear();


		vel::Renderer::BeginScene(editorCamera.GetViewProjection());

		m_Texture->Bind(0);
		vel::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));

		m_SecondTexture->Bind(0);
		vel::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));

		vel::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{

	}

	void OnEvent(vel::Event& event) override
	{
		editorCamera.OnEvent(event);
		if (event.GetEventType() == vel::EventType::WindowResize)
		{
			auto& we = (vel::WindowResizeEvent&)event;
			editorCamera.SetViewportSize(we.GetWidth(), we.GetHeight());
		}
	}

	private:
		vel::ShaderLibrary m_ShaderLibrary;
		vel::Ref<vel::Shader> m_Shader2;
		vel::Ref<vel::Texture2D> m_Texture;
		vel::Ref<vel::Texture2D> m_SecondTexture;
		vel::Ref<vel::VertexArray> m_SquareVertexArray;
		vel::EditorCamera editorCamera;
	};

class VelApp : public vel::Application
{
public:
	VelApp()
	{
		//PushLayer(new ExampleLayer());
		//PushLayer(new ExampleLayer2());
		PushLayer(new PhysicsScene());
	}

	~VelApp()
	{

	}
};

vel::Application* vel::CreateApplication()
{
	return new VelApp();
}
