#include "EditorLayer.h"

#include <chrono>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

namespace vel
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_EditorCamera(45.f, 1280, 720, 0.1f, 10000.f)
	{

	}

	void EditorLayer::OnAttach()
	{
		FrameBufferSpecification fbSpec;
		fbSpec.Width = Application::Get().GetWindow().GetWidth();
		fbSpec.Height = Application::Get().GetWindow().GetHeight();
		m_RenderBuffer = FrameBuffer::Create(fbSpec);

		m_FullScreenFrameBuffer = FrameBuffer::Create(fbSpec);

		m_ShaderLibrary.Load("assets/shaders/DeferredShader.glsl");
		m_ShaderLibrary.Get("DeferredShader")->Bind();
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gAlbedoSpec", 0);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gPosition", 1);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gNormal", 2);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gSpecular", 3);

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

		Renderer::Init();
		RenderCommand::SetCullFace();

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
			RenderCommand::SetClearColor(glm::vec4(0.8f, 0.2f, 0.8f, 1));
			RenderCommand::Clear();
			RenderCommand::EnableDepth();
		}

		{
			VEL_PROFILE_SCOPE("Renderer Draw");
			Renderer::BeginScene(m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetUnReversedProjectionMatrix());

			m_SceneManager.Update(ts, glm::vec4(m_EditorCamera.GetPosition(), 1.0));

			Renderer::EndScene();

			m_RenderBuffer->Unbind();

			//RenderCommand::EnableDepth();
			//m_FullScreenFrameBuffer->CopyDepthData(m_RenderBuffer);
			m_FullScreenFrameBuffer->Bind();
			//m_FullScreenFrameBuffer->Bind();


			m_ShaderLibrary.Get("DeferredShader")->Bind();
			m_ShaderLibrary.Get("DeferredShader")->
				SetFloat2("screen_width_height", { 
				Application::Get().GetWindow().GetWidth(),Application::Get().GetWindow().GetHeight() 
					});
			m_RenderBuffer->BindColorTexture();
			m_RenderBuffer->BindWorldPositionTexture();
			m_RenderBuffer->BindNormalTexture();
			m_RenderBuffer->BindSpecularTexture();
			m_SceneManager.BindLightData(m_ShaderLibrary.Get("DeferredShader"), glm::vec4(m_EditorCamera.GetPosition(), 1.0));

			Renderer::Submit(
				m_ShaderLibrary.Get("DeferredShader"),
				m_SquareVertexArray);

			// To render the skybox properly
			m_FullScreenFrameBuffer->CopyDepthData(m_RenderBuffer);

			m_FullScreenFrameBuffer->Bind();
			m_SceneManager.DrawSkyBox(m_EditorCamera.GetUnReversedProjectionMatrix() * m_EditorCamera.GetViewMatrix() * glm::scale(glm::mat4(1.0), glm::vec3(9000.f)));
			
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
				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					openProjectCalled = true;

				ImGui::Separator();

				ImGui::MenuItem("New Scene", "Ctrl+N");
				//NewScene();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					m_SceneManager.SaveScene();

				if(ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					saveProjectCalled = true;

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

		if (openProjectCalled)
			OpenProject();
		if (saveProjectCalled)
			SaveScene();

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
		Inspector();
	}

	void EditorLayer::SceneHierarchy()
	{
		ImGui::ShowDemoWindow();
		ImGui::Begin("Scene Hierachy");
		std::vector<Ref<Entity>> entityList = m_SceneManager.GetEntityManager()->GetAllEntities();
		for(int i = 0; i < entityList.size(); i++)
		{
			char label[50];
			strcpy(label, entityList.at(i)->name.c_str());
			//std::string latter = "##" + std::to_string(entityList[i]->GetID());
			//strcat(label, latter.c_str());

			if (ImGui::Selectable(label, m_SelectedEntity == i))
			{
				m_SelectedEntity = i;
			}
		}
		if (ImGui::Button("Add Entity"))
		{
			int number = m_SceneManager.GetEntityManager()->GetAllEntities().size();
			m_SceneManager.GetEntityManager()->CreateEntity("New Entity" + std::to_string(number));
		}
		ImGui::End();
	}

	void EditorLayer::Inspector()
	{
		bool hasMesh = false;
		bool hasLight = false;
		ImGui::Begin("Inspector");
		if (m_SelectedEntity != -1)
		{
			std::vector<Ref<Entity>> entityList = m_SceneManager.GetEntityManager()->GetAllEntities();
			Ref<Entity> entity = entityList.at(m_SelectedEntity);
			ImGui::BeginGroup();
			/*--Position*/
			std::string goName = entity->name;
			ImGui::InputText("##Gameobject",
				&goName);
			if (ImGui::IsItemDeactivated())
			{
				if (!goName.empty())
				{
					entity->name = goName;
				}
			}
			ImGui::SameLine();
			ImGui::Checkbox("##enabled", &entity->enabled);
			ImGui::SameLine();
			ImGui::Button("Duplicate");
			if (ImGui::IsItemClicked())
			{
				//this->DuplicateGameObject(entity, shaderID);
			}

			ImGui::Separator();

			{
				ImGui::BulletText("Transform");
				{
					ImGui::Text("Position");
					ImGui::SameLine();
					TransformComponent* transform = 
						m_SceneManager.GetEntityManager()->
						GetComponentByType<TransformComponent>(entity->GetID());

					ImGui::InputFloat3("##pos", glm::value_ptr(transform->Translation));

					ImGui::Text("Rotation");
					ImGui::SameLine();

					ImGui::InputFloat3("##rot", glm::value_ptr(transform->Rotation));

					ImGui::Text("Scale");
					ImGui::SameLine();

					ImGui::InputFloat3("##sca", glm::value_ptr(transform->Scale));
				}
				ImGui::EndGroup();
			}

			ImGui::BeginGroup();
			{
				if (m_SceneManager.GetEntityManager()->HasComponent< LightComponent>(entity->GetID()))
				{
					hasLight = true;
					LightComponent* light =
						m_SceneManager.GetEntityManager()->
						GetComponentByType<LightComponent>(entity->GetID());

					ImGui::Separator();
					ImGui::BulletText("Light##lightLight");
					ImGui::SameLine();
					ImGui::Checkbox("##EnableLight", &light->Enabled);
					ImGui::ColorEdit4("Diffuse##lightDiffuse", glm::value_ptr(light->Diffuse));
					ImGui::ColorEdit4("Specular##lightSpec", glm::value_ptr(light->Specular));
					ImGui::Text("Attenuation##lightAtt");
					ImGui::SliderFloat("Constant##lightAttconst", &light->Attenuation[0], 0.f, 1.0f);
					ImGui::SliderFloat("Linear##lightAttline", &light->Attenuation[1], 0.f, 1.0f);
					ImGui::SliderFloat("Quadratic##lightAttquad", &light->Attenuation[2], 0.f, 0.3f);
					ImGui::InputFloat("Type##lightType", &light->LightParams[0]);
					ImGui::InputFloat("inner Cutt Off##lightCuttoff", &light->LightParams[1]);
					ImGui::InputFloat("Outter Cut Off##lightOutCuttoff", &light->LightParams[2]);
					ImGui::InputFloat("On Off##lightOnoff", &light->LightParams[3]);
				}
			}
			ImGui::EndGroup();

			ImGui::BeginGroup();
			{
				if (m_SceneManager.GetEntityManager()->HasComponent< MeshComponent>(entity->GetID()))
				{
					hasMesh = true;
					MeshComponent* mesh =
						m_SceneManager.GetEntityManager()->
						GetComponentByType<MeshComponent>(entity->GetID());

					ImGui::Separator();
					ImGui::BulletText("Mesh Renderer");
					ImGui::SameLine();
					ImGui::Checkbox("##EnableRenderert", &mesh->Enabled);
					ImGui::InputText("Mesh Path", &mesh->Path);
					ImGui::Checkbox("Use FBX Textures", &mesh->UseFBXTextures);
					ImGui::SameLine();
					if (ImGui::Button("Load Mesh"))
					{
						mesh->ModelIns = CreateRef<Model>(mesh->Path, mesh->UseFBXTextures, false);
					}
					ImGui::Separator();
					ImGui::BulletText("Material");
					ImGui::InputText("Name", &mesh->MaterialIns->Name);
					ImGui::InputText("Path", &mesh->MaterialPath);
					ImGui::ColorEdit4("Diffuse##difmaterial", glm::value_ptr(mesh->MaterialIns->Diffuse));
					ImGui::ColorEdit4("Specular##specmaterial", glm::value_ptr(mesh->MaterialIns->Specular));
					ImGui::InputFloat("Ambient##ambmaterial", &mesh->MaterialIns->Ambient);
					ImGui::SliderFloat("Shininess##ambmaterial", &mesh->MaterialIns->Shininess, 0, 1.0f);

					if (mesh->MaterialIns->DiffuseTexture && mesh->MaterialIns->DiffuseTexture->IsLoaded())
					{
						ImGui::Image((void*)mesh->MaterialIns->DiffuseTexture->GetRendererID(), ImVec2{ 128, 128 });
						if (ImGui::Button("Unload##btndiffUNLoad"))
						{
							mesh->MaterialIns->DiffuseTexture->RemoveData();
						}
					}
					else
					{
						ImGui::InputText("Diffuse Texture##difTexmaterial", &mesh->MaterialIns->DiffuseTexturePath);
						ImGui::SameLine();
						if (ImGui::Button("Load##btndiffLoad"))
						{
							if (mesh->MaterialIns->DiffuseTexture)
							{
								mesh->MaterialIns->DiffuseTexture->CreateData(mesh->MaterialIns->DiffuseTexturePath);
							}
							else
							{
								Ref<Texture2D> texture = Texture2D::Create(mesh->MaterialIns->DiffuseTexturePath);
								if (texture->IsLoaded())
								{
									mesh->MaterialIns->DiffuseTexture = texture;
								}
							}
						}
					}
					
					if (mesh->MaterialIns->SpecularTexture && mesh->MaterialIns->SpecularTexture->IsLoaded())
					{
						ImGui::Image((void*)mesh->MaterialIns->SpecularTexture->GetRendererID(), ImVec2{ 128, 128 });
						if (ImGui::Button("Unload##btnspecUNLoad"))
						{
							mesh->MaterialIns->SpecularTexture->RemoveData();
						}
					}
					else
					{
						ImGui::InputText("Specular Texture##specTexmaterial", &mesh->MaterialIns->SpecularTexturePath);
						ImGui::SameLine();
						if (ImGui::Button("Load##btnspecLoad"))
						{
							if (mesh->MaterialIns->SpecularTexture)
							{
								mesh->MaterialIns->SpecularTexture->CreateData(mesh->MaterialIns->SpecularTexturePath);
							}
							else
							{
								Ref<Texture2D> texture = Texture2D::Create(mesh->MaterialIns->SpecularTexturePath);
								if (texture->IsLoaded())
								{
									mesh->MaterialIns->SpecularTexture = texture;
								}
							}
						}
					}
					
					if (mesh->MaterialIns->NormalTexture && mesh->MaterialIns->NormalTexture->IsLoaded())
					{
						ImGui::Image((void*)mesh->MaterialIns->NormalTexture->GetRendererID(), ImVec2{ 128, 128 });
						if (ImGui::Button("Unload##btnnormUNLoad"))
						{
							mesh->MaterialIns->NormalTexture->RemoveData();
						}
					}
					else
					{
						ImGui::InputText("Normal Texture##normTexmaterial", &mesh->MaterialIns->NormalTexturePath);
						ImGui::SameLine();
						if (ImGui::Button("Load##btnnormLoad"))
						{
							if (mesh->MaterialIns->NormalTexture)
							{
								mesh->MaterialIns->NormalTexture->CreateData(mesh->MaterialIns->NormalTexturePath);
							}
							else
							{
								Ref<Texture2D> texture = Texture2D::Create(mesh->MaterialIns->NormalTexturePath);
								if (texture->IsLoaded())
								{
									mesh->MaterialIns->NormalTexture = texture;
								}
							}
						}
					}
				}
			}
			ImGui::EndGroup();

			{
				if (ImGui::Button("Add Component"))
				{
					if (!hasLight || !hasMesh)
					{
						addComponentCalled = true;
						ImGui::OpenPopup("Add Component");
					}
				}
				if (addComponentCalled)
				{
					if (ImGui::BeginPopup("Add Component"))
					{
						if (!hasLight)
						{
							if (ImGui::MenuItem("Light Component"))
							{
								addComponentCalled = false;
								m_SceneManager.GetEntityManager()->AddComponent(entity->GetID(), new LightComponent());
								ImGui::CloseCurrentPopup();
							}
						}
						if (!hasMesh)
						{
							if (ImGui::MenuItem("Mesh Component"))
							{
								addComponentCalled = false;
								m_SceneManager.GetEntityManager()->AddComponent(entity->GetID(), new MeshComponent());
								ImGui::CloseCurrentPopup();
							}
						}
						if (ImGui::MenuItem("Close"))
						{
							addComponentCalled = false;
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
				}
			}
		}
		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
	}

	void EditorLayer::OpenProject()
	{
		ImGui::OpenPopup("Open Project");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Open Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Project Name", &m_SceneManager.ScenePath);
			if (ImGui::Button("Load"))
			{
				m_SceneManager.LoadScene();
				openProjectCalled = false;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel"))
			{
				openProjectCalled = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void EditorLayer::SaveScene()
	{
		ImGui::OpenPopup("Save Project");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Save Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Project Name", &m_SceneManager.ScenePath);
			if (ImGui::Button("Save"))
			{
				m_SceneManager.SaveScene();
				saveProjectCalled = false;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel"))
			{
				saveProjectCalled = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
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
