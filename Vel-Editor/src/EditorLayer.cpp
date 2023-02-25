#include "EditorLayer.h"

#include <chrono>
#include <imgui/imgui.h>

namespace vel
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_EditorCamera(45.f, 1280, 720, 0.1f, 1000.f)
	{

	}

	void EditorLayer::OnAttach()
	{
		FrameBufferSpecification fbSpec;
		fbSpec.Width = Application::Get().GetWindow().GetWidth();
		fbSpec.Height = Application::Get().GetWindow().GetHeight();
		m_RenderBuffer = FrameBuffer::Create(fbSpec);

		m_FullScreenFrameBuffer = FrameBuffer::Create(fbSpec);

		m_ShaderLibrary.Load("assets/shaders/FBOTexture.glsl");
		m_ShaderLibrary.Get("FBOTexture")->Bind();
		m_ShaderLibrary.Get("FBOTexture")->SetInt("u_Texture", 0);

		m_SquareVertexArray = VertexArray::Create();
		float sqVertices[6 * 4] = {
			0.0f, 0.0f,	-1.f,	-1.f,	0.0f,	1.f,
			1.0f, 0.0f, 1.f,	-1.f,	0.0f,	1.f,
			1.0f, 1.0f, 1.f,	1.f,	0.0f,	1.f,
			0.0f, 1.0f, -1.f,	1.f,	0.0f,	1.f
		};
		m_SquareVertexArray = VertexArray::Create();
		Ref<VertexBuffer> squareVB;
		squareVB = (VertexBuffer::Create(sqVertices, sizeof(sqVertices)));

		squareVB->SetLayout({
				{ ShaderDataType::Float2, "a_TextureCoords"},
				{ ShaderDataType::Float4, "a_Position"}
			});
		m_SquareVertexArray->AddVertexBuffer(squareVB);

		uint32_t sq_indices[6] = {
			0, 1, 2, 2, 3, 0
		};

		// index buffer
		Ref<IndexBuffer> squareIB;
		squareIB = (IndexBuffer::Create(sq_indices, sizeof(sq_indices) / sizeof(uint32_t)));
		m_SquareVertexArray->SetIndexBuffer(squareIB);
		//m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
		m_Texture = Texture2D::Create(1, 1);
		uint32_t whiteTexture = 0xffffff87;
		m_Texture->SetData(&whiteTexture, sizeof(uint32_t));
		m_SecondTexture = Texture2D::Create("assets/textures/1277885.png");
		Renderer::Init();
		/*m_ShaderLibrary.Get("Texture")->Bind();
		m_ShaderLibrary.Get("Texture")->SetInt("u_Texture", 0);*/

	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		VEL_PROFILE_FUNCTION();
		{
			VEL_PROFILE_SCOPE("EditorCamera::OnUpdate");
			if(m_ViewportFocused)
				m_EditorCamera.OnUpdate(ts);
		}

		{
			VEL_PROFILE_SCOPE("Renderer Prep");
			m_RenderBuffer->Bind();
			RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
			RenderCommand::Clear();
		}

		{
			VEL_PROFILE_SCOPE("Renderer Draw");
			Renderer::BeginScene(m_EditorCamera.GetViewProjection());

			m_Texture->Bind(0);
			Renderer::Submit(m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));

			m_SecondTexture->Bind(0);
			Renderer::Submit(m_SquareVertexArray, glm::scale(glm::mat4(1.f), glm::vec3(5.5f)));


			Renderer::EndScene();

			m_RenderBuffer->Unbind();

			m_FullScreenFrameBuffer->Bind();
			RenderCommand::DisableDepth();
			RenderCommand::Clear();
			m_RenderBuffer->BindColorTexture();
			vel::Renderer::Submit(
				m_ShaderLibrary.Get("FBOTexture"),
				m_SquareVertexArray);

			m_FullScreenFrameBuffer->Unbind();

		}
	}

	void EditorLayer::OnImGuiRender()
	{
		VEL_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("Open Project...", "Ctrl+O");
				//OpenProject();

				ImGui::Separator();

				ImGui::MenuItem("New Scene", "Ctrl+N");
				//NewScene();

				ImGui::MenuItem("Save Scene", "Ctrl+S");
				//SaveScene();

				ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S");
				//SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			/*if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
					ScriptEngine::ReloadAssembly();

				ImGui::EndMenu();
			}*/

			ImGui::EndMenuBar();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewPortSize != *(glm::vec2*)&viewportPanelSize)
		{
			m_RenderBuffer->Resize(viewportPanelSize.x, viewportPanelSize.y);
			m_FullScreenFrameBuffer->Resize(viewportPanelSize.x, viewportPanelSize.y);
			m_EditorCamera.SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);
			m_ViewPortSize = { viewportPanelSize.x, viewportPanelSize.y };
		}

		uint32_t textureID = m_FullScreenFrameBuffer->GetColorAttachmenRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{0,1}, ImVec2{1,0});
		ImGui::End();
		ImGui::PopStyleVar();
		SceneHierarchy();
	}

	void EditorLayer::SceneHierarchy()
	{
		ImGui::ShowDemoWindow();
		ImGui::Begin("Scene Hierachy");
		std::vector<Entity*> entityList = m_SceneManager.GetEntityManager().GetAllEntities();
		for(int i = 0; i < entityList.size(); i++)
		{
			char label[50];
			strcpy(label, entityList[i]->name.c_str());
			//std::string latter = "##" + std::to_string(entityList[i]->GetID());
			//strcat(label, latter.c_str());

			if (ImGui::Selectable(label, m_SelectedEntity == i))
			{
				m_SelectedEntity = i;
			}
		}
		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_EditorCamera.OnEvent(event);
		if (event.GetEventType() == EventType::WindowResize)
		{
			auto& we = (WindowResizeEvent&)event;
			m_EditorCamera.SetViewportSize(we.GetWidth(), we.GetHeight());
		}
	}
}
