#include "EditorLayer2.h"

#include <chrono>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>
#include <CharacterControllerDesc.h>

namespace vel
{
	EditorLayer2::EditorLayer2()
		: Layer("EditorLayer2"), m_EditorCamera(45.f, 1280, 720, 0.1f, 10000.f)
	{

	}
	static glm::vec4 camPos = glm::vec4(0.f, 30.f, -20.f, 1.0);
	void EditorLayer2::OnAttach()
	{
		FrameBufferSpecification fbSpec;
		fbSpec.Width = Application::Get().GetWindow().GetWidth();
		fbSpec.Height = Application::Get().GetWindow().GetHeight();
		m_RenderBuffer = FrameBuffer::Create(fbSpec);

		m_FullScreenFrameBuffer = FrameBuffer::Create(fbSpec);
		m_PostProcessFrameBuffer = FrameBuffer::Create(fbSpec);
		m_ShadowBuffer = FrameBuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		m_ShaderLibrary.Load("assets/shaders/DeferredShader.glsl");
		m_ShaderLibrary.Load("assets/shaders/PostProcessing.glsl");
		m_ShaderLibrary.Load("assets/shaders/GaussianBlur.glsl");
		m_ShaderLibrary.Get("GaussianBlur")->Bind();
		m_ShaderLibrary.Get("GaussianBlur")->SetInt("screenTexture", 4);

		m_ShaderLibrary.Get("DeferredShader")->Bind();
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gAlbedoSpec", 0);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gPosition", 1);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gNormal", 2);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gSpecular", 3);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gEmissive", 4);
		m_ShaderLibrary.Get("DeferredShader")->SetInt("gBloom", 5);

		m_ShaderLibrary.Get("PostProcessing")->Bind();
		m_ShaderLibrary.Get("PostProcessing")->SetInt("gAlbedoSpec", 0);
		m_ShaderLibrary.Get("PostProcessing")->SetInt("gPosition", 1);
		m_ShaderLibrary.Get("PostProcessing")->SetInt("gNormal", 2);
		m_ShaderLibrary.Get("PostProcessing")->SetInt("gSpecular", 3);
		m_ShaderLibrary.Get("PostProcessing")->SetInt("gEmissive", 4);
		m_ShaderLibrary.Get("PostProcessing")->SetInt("gBloom", 5);
		m_ShaderLibrary.Get("PostProcessing")->SetInt("gNoise", 6);

		//m_SquareVertexArray = VertexArray::Create();
		//float sqVertices[6 * 4] = {
		//	0.0f, 0.0f,	-1.f,	-1.f,	0.0f,	1.f,
		//	1.0f, 0.0f, 1.f,	-1.f,	0.0f,	1.f,
		//	1.0f, 1.0f, 1.f,	1.f,	0.0f,	1.f,
		//	0.0f, 1.0f, -1.f,	1.f,	0.0f,	1.f
		//};
		//m_SquareVertexArray = VertexArray::Create();
		//Ref<VertexBuffer> squareVB;
		//squareVB = (VertexBuffer::Create(sqVertices, sizeof(sqVertices)));
		//
		//squareVB->SetLayout({
		//		{ ShaderDataType::Float2, "a_TextureCoords"},
		//		{ ShaderDataType::Float4, "a_Position"}
		//	});
		//m_SquareVertexArray->AddVertexBuffer(squareVB);
		//
		//uint32_t sq_indices[6] = {
		//	0, 1, 2, 2, 3, 0
		//};
		//
		//// index buffer
		//Ref<IndexBuffer> squareIB;
		//squareIB = (IndexBuffer::Create(sq_indices, sizeof(sq_indices) / sizeof(uint32_t)));
		//m_SquareVertexArray->SetIndexBuffer(squareIB);

		Renderer::Init();
		RenderCommand::SetCullFace();
	}

	void EditorLayer2::OnDetach()
	{
	}

	void EditorLayer2::OnUpdate(Timestep ts)
	{
		VEL_PROFILE_FUNCTION();
		{
			VEL_PROFILE_SCOPE("EditorCamera::OnUpdate");
			if (m_ViewportFocused)
				m_EditorCamera.OnUpdate(ts);
		}
		if (controller)
		{
			if (Input::IsKeyPressed(KeyCode::W))
				controller->AddForce({ 0.f, 0.f, 5.f });
		}
		{
			VEL_PROFILE_SCOPE("Renderer Prep");
			m_RenderBuffer->Bind();
			RenderCommand::SetClearColor(glm::vec4(0.f, 0.f, 0.f, 1));
			RenderCommand::Clear();
			RenderCommand::EnableDepth();
		}

		{
			VEL_PROFILE_SCOPE("Renderer Draw");
			Renderer::BeginScene(m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetUnReversedProjectionMatrix());

			m_ActiveScene->OnUpdateEditor(ts, glm::vec4(m_EditorCamera.GetPosition(), 1.f));

			Renderer::EndScene();

			m_RenderBuffer->Unbind();

			m_FullScreenFrameBuffer->Bind();
			RenderCommand::Clear();
			m_FullScreenFrameBuffer->EnableGammaCorrection();

			//------------------------------------	

			m_ShaderLibrary.Get("DeferredShader")->Bind();
			m_ShaderLibrary.Get("DeferredShader")->
				SetFloat2("screen_width_height", {
				Application::Get().GetWindow().GetWidth(),Application::Get().GetWindow().GetHeight()
					});
			m_RenderBuffer->BindColorTexture();
			m_RenderBuffer->BindWorldPositionTexture();
			m_RenderBuffer->BindNormalTexture();
			m_RenderBuffer->BindSpecularTexture();
			m_RenderBuffer->BindEmissiveTexture();
			m_RenderBuffer->BindBloomTexture();
			m_ShaderLibrary.Get("DeferredShader")->SetFloat("Exposure", m_ActiveScene->Exposure);
			m_ShaderLibrary.Get("DeferredShader")->SetFloat("BloomThreshold", m_ActiveScene->BloomThreshold);
			m_ActiveScene->BindLightData(m_ShaderLibrary.Get("DeferredShader"), glm::vec4(m_EditorCamera.GetPosition(), 1.0));

			Renderer::DrawFullscreenQuad(
				m_ShaderLibrary.Get("DeferredShader"));

			m_FullScreenFrameBuffer->CopyDepthData(m_RenderBuffer);
			m_FullScreenFrameBuffer->Bind();

			m_ActiveScene->DrawSkyBox(m_EditorCamera.GetUnReversedProjectionMatrix() * m_EditorCamera.GetViewMatrix() * glm::scale(glm::mat4(1.0), glm::vec3(9000.f)));

			m_FullScreenFrameBuffer->Unbind();
			//****************** POST PROCESSING ************************
			m_PostProcessFrameBuffer->Bind();
			RenderCommand::Clear();
			m_ShaderLibrary.Get("PostProcessing")->Bind();
			m_ShaderLibrary.Get("PostProcessing")->
				SetFloat2("screen_width_height", {
				Application::Get().GetWindow().GetWidth(),Application::Get().GetWindow().GetHeight()
					});
			m_FullScreenFrameBuffer->BindColorTexture();
			m_FullScreenFrameBuffer->BindWorldPositionTexture();
			m_FullScreenFrameBuffer->BindNormalTexture();
			m_FullScreenFrameBuffer->BindSpecularTexture();
			m_FullScreenFrameBuffer->BindEmissiveTexture();
			m_FullScreenFrameBuffer->BindBloomTexture();

			m_ShaderLibrary.Get("PostProcessing")->SetFloat4("eyeLocation", glm::vec4(m_EditorCamera.GetPosition(), 1.f));
			m_ShaderLibrary.Get("PostProcessing")->SetBool("DepthOfField", m_ActiveScene->DepthOfField);
			m_ShaderLibrary.Get("PostProcessing")->SetFloat("FocalDistance", m_ActiveScene->FocalDistance);

			m_ShaderLibrary.Get("PostProcessing")->SetBool("UseBloom", m_ActiveScene->UseBloom);
			m_ShaderLibrary.Get("PostProcessing")->SetFloat("BloomIntensity", m_ActiveScene->BloomIntensity);

			std::chrono::duration<float> time = (std::chrono::steady_clock::now() - starttime);
			m_ShaderLibrary.Get("PostProcessing")->SetFloat("time", ts.GetSeconds());
			m_ShaderLibrary.Get("PostProcessing")->SetFloat("timeInc", time.count());
			m_ShaderLibrary.Get("PostProcessing")->SetBool("nightVision", m_ActiveScene->useNightVision);
			m_ShaderLibrary.Get("PostProcessing")->SetBool("rippleEffect", m_ActiveScene->useRippleEffect);
			m_ShaderLibrary.Get("PostProcessing")->SetBool("grayscale", m_ActiveScene->useGrayscale);

			m_ShaderLibrary.Get("PostProcessing")->SetFloat2("screen_width_height", { m_ViewPortSize.x, m_ViewPortSize.y });

			Renderer::DrawFullscreenQuad(
				m_ShaderLibrary.Get("PostProcessing"));

			m_PostProcessFrameBuffer->CopyDepthData(m_FullScreenFrameBuffer);
			m_PostProcessFrameBuffer->Unbind();

		}
	}

	void EditorLayer2::OnImGuiRender()
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

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{
					//NewScene();
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					m_ActiveScene->SaveScene();

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					saveProjectCalled = true;

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();

		if (openProjectCalled)
			OpenProject();
		if (saveProjectCalled)
			SaveProject();

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

		uint32_t textureID = m_PostProcessFrameBuffer->GetColorAttachmenRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
		ImGui::End();
		ImGui::PopStyleVar();
		SceneHierarchy();
		Inspector();

		{
			ImGui::Begin("Post Processing and Shader");
			if (ImGui::Button("Reload Shader##reloadShaderbtn"))
			{
				m_ActiveScene->ReloadShader();
			}

			ImGui::InputFloat("Exposure##sceneExp", &m_ActiveScene->Exposure);
			ImGui::Checkbox("Depth Of Field##sceneDOF", &m_ActiveScene->DepthOfField);
			ImGui::SliderFloat("Focal Distance##scenefocalDistance", &m_ActiveScene->FocalDistance, 0.1f, 50.f);

			ImGui::Checkbox("Use Bloom##scenebloom", &m_ActiveScene->UseBloom);
			ImGui::SliderFloat("Bloom Intensity##scenebloomIntes", &m_ActiveScene->BloomIntensity, 0.f, 0.2f);
			ImGui::SliderFloat("Bloom Threshold##scenebloomThres", &m_ActiveScene->BloomThreshold, 0.f, 20.f);

			ImGui::Checkbox("Use Night Vision##sceneNightVision", &m_ActiveScene->useNightVision);
			ImGui::Checkbox("Use Ripple Effect##sceneRippleEffect", &m_ActiveScene->useRippleEffect);
			ImGui::Checkbox("Use Grayscale##sceneGrayscale", &m_ActiveScene->useGrayscale);

			ImGui::End();
		}

		// Render Passes
		{
			ImGui::Begin("Bloom");
			uint32_t BloomID = m_FullScreenFrameBuffer->GetBloomAttachmenRendererID();
			ImGui::Image((void*)BloomID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
			ImGui::End();
		}
	}

	void EditorLayer2::DrawChildren(Entity entity, std::map<GUID, Entity>& shownEntities)
	{
		char label[50];
		strcpy(label, entity.Name().c_str());
		if (entity.Children().size() > 0)
		{
			std::vector<GUID> children = entity.Children();
			if (ImGui::TreeNode(label))
			{
				if (ImGui::IsItemClicked())
				{
					m_SelectedEntity = entity;
				}
				shownEntities[entity.GetGUID()] = entity;
				for (int k = 0; k < children.size(); k++)
				{
					Entity childEntity = Entity(m_ActiveScene->GetEntityMap()[children[k]], m_ActiveScene.get());
				
					
					DrawChildren(childEntity, shownEntities);
				}
				ImGui::TreePop();
			}
		}
		else
		{
			shownEntities[entity.GetGUID()] = entity;
			if (m_SelectedEntity == entity)
			{
				ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), label);
			}
			else ImGui::Text(label);
			if (ImGui::IsItemClicked())
			{
				m_SelectedEntity = entity;
			}
		}
		
	}
	void EditorLayer2::SceneHierarchy()
	{
		ImGui::Begin("Scene Hierachy");
		auto entities = m_ActiveScene->GetAllEntitiesWith<TransformComponent>();
		int l = entities.size();
		std::map<GUID, Entity> shownEntities;
		for (auto entityHandle : entities)
		{
			Entity entity = Entity(entityHandle, m_ActiveScene.get());

			if (entity.GetParentGUID() != 0)
				continue;

			if (shownEntities.find(entity.GetGUID()) != shownEntities.end())
				continue;
			char label[50];
			strcpy(label, entity.Name().c_str());
		
			if (entity.Children().size() > 0)
			{
				DrawChildren(entity, shownEntities);
				/*if (ImGui::TreeNode(label))
				{
					if (ImGui::IsItemClicked())
					{
						m_SelectedEntity = entity;
					}
					shownEntities[entity.GetGUID()] = entity;
					ImGui::TreePop();
				}*/
			}
			else
			{
				shownEntities[entity.GetGUID()] = entity;
				if (ImGui::Selectable(label, m_SelectedEntity == entity))
				{
					m_SelectedEntity = entity;
				}
			}
		}
		if (ImGui::Button("Add Entity"))
		{
			m_ActiveScene->CreateEntity();
		}
		ImGui::End();
	}

	void EditorLayer2::Inspector()
	{
		bool hasMesh = false;
		bool hasAnimator = false;
		bool hasAsset = false;
		bool hasLight = false;
		bool hasCC = false;
		ImGui::Begin("Inspector");
		if (m_SelectedEntity != entt::null)
		{
			Entity entity = Entity(m_SelectedEntity, m_ActiveScene.get());

			ImGui::BeginGroup();
			std::string goName = entity.Name();
			ImGui::InputText("##Gameobject",
				&goName);
			if (ImGui::IsItemDeactivated())
			{
				if (!goName.empty())
				{
					entity.Name() = goName;
				}
			}
			ImGui::SameLine();
			ImGui::Checkbox("##enabled", &entity.Transform().enabled);
			ImGui::SameLine();
			ImGui::Button("Duplicate");
			ImGui::Text(std::to_string(entity.GetGUID()).c_str());
			ImGui::Separator();

			// Transform
			{
				ImGui::BulletText("Transform");
				{
					/*--Position*/
					ImGui::Text("Position");
					ImGui::SameLine(); 
					TransformComponent* transform = &entity.Transform();
						
					ImGui::InputFloat3("##pos", glm::value_ptr(transform->Translation));
					
					ImGui::Text("Rotation");
					ImGui::SameLine();
					ImGui::InputFloat3("##rot", glm::value_ptr(transform->RotationEuler));

					ImGui::Text("Scale");
					ImGui::SameLine();
					ImGui::InputFloat3("##sca", glm::value_ptr(transform->Scale));
				}
			}
			ImGui::EndGroup();

			// Light
			ImGui::BeginGroup();
			{
				if (entity.HasComponent< LightComponent>())
				{
					hasLight = true;
					LightComponent* light =
						&entity.GetComponent<LightComponent>();

					ImGui::Separator();
					ImGui::BulletText("Light");
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
					ImGui::SliderFloat("Intensity##lightintensity", &light->Intensity, 0.f, 100.0f);
					if (ImGui::Button("Copy Component"))
					{
						//copiedLight = *light;
					}
					if (ImGui::Button("Paste Component"))
					{
						//*light = copiedLight;
					}
				}
			}
			ImGui::EndGroup();

			// Asset
			ImGui::BeginGroup();

			{
				if (entity.HasComponent<AssetComponent>())
				{
					hasAsset = true;
					AssetComponent* asset = &entity.GetComponent<AssetComponent>();
					ImGui::InputText("Asset Location##assetLoc", &asset->FileLocation);
					if (ImGui::Button("Load Asset"))
					{
						if (!asset->FileLocation.empty())
						{
							std::map<GUID, Asset> assetsData;
							assetsData = LoadAssetFile(asset->FileLocation + ".vasset");
							ModelLoadData data;
							data.material = MaterialSystem::defaultMaterial;
							data.gameScene = m_ActiveScene.get();
							data.useTextures = false;
							data.loadAsync = true;
							data.source = asset->FileLocation;
							data.parentEntity = &entity;
							data.assetData = assetsData;
							asset->FileLocation += ".vasset";
							MeshRenderer::LoadMesh(data);
						}
					}
				}
			}

			ImGui::EndGroup();

			// Animator
			ImGui::BeginGroup();

			{
				if (entity.HasComponent<AnimatorComponent>())
				{
					AnimatorComponent& animator = entity.GetComponent<AnimatorComponent>();
					hasAnimator = true;
					ImGui::InputText("AnimationPath##textInputAnimPath", &animator.AnimationPath);
					if (ImGui::Button("Load Animation##btnLoadAnim"))
					{
						MeshComponent& mesh = entity.GetComponent<MeshComponent>();
						animator.LoadAnimation(mesh.MeshDrawData);
						//m_ActiveScene->LoadAnimation(animator.animation);
					}
					for (Animation* animation : animator.List_Animations)
					{
						ImGui::TextColored({0.8f, 0.8f, 0.2f, 1.f}, animation->name.c_str());
						ImGui::SameLine();
						std::string label = "Play##" + animation->name + "Playbtn";
						if (ImGui::Button(label.c_str()))
						{
							animator.animator->PlayAnimation(animation);
						}
					}
				}
			}

			ImGui::EndGroup();

			// Character Controller
			ImGui::BeginGroup();

			{
				if (entity.HasComponent<CharacterControllerComponent>())
				{
					hasCC = true;
					CharacterControllerComponent characterController = entity.GetComponent<CharacterControllerComponent>();
					ImGui::InputFloat("Radius##ccRadius", &characterController.radius);
					ImGui::InputFloat("Height##ccHeight", &characterController.height);
				}
			}

			ImGui::EndGroup();

			ImGui::BeginGroup();
			{
				if (entity.HasComponent< MeshComponent>())
				{
					hasMesh = true;
					MeshComponent* mesh =
						&entity.GetComponent< MeshComponent>();

					ImGui::Separator();
					ImGui::BulletText("Mesh Renderer");
					ImGui::BulletText("Material");
					Ref<Material> material = MaterialSystem::GetMaterial(mesh->MaterialPath);
					if (!material)
					{
						material = mesh->MaterialIns;
						ImGui::InputText("Name", &material->Name);
						ImGui::InputText("Path", &mesh->MaterialPath);
						if (ImGui::Button("Load Material##matLoadBtn"))
						{
							Ref<Material> mat = MaterialSystem::GetMaterial(mesh->MaterialPath);
							if (mat)
								mesh->MaterialIns = mat;
						}
						if (ImGui::Button("Create Material##matCreateBtn"))
						{
							if (!mesh->MaterialPath.empty())
							{
								mesh->MaterialIns = MaterialSystem::LoadMaterial(mesh->MaterialPath, CreateRef<Material>());
							}
						}
					}
					else{
						ImGui::InputText("Name", &material->Name);
						ImGui::InputText("Path", &mesh->MaterialPath);
						if (ImGui::Button("Load Material##matLoadBtn"))
						{
							Ref<Material> mat = MaterialSystem::GetMaterial(mesh->MaterialPath);
							if (mat)
								mesh->MaterialIns = mat;
						}
						if (ImGui::Button("Save Material##matSaveBtn"))
						{
							if (!mesh->MaterialPath.empty() && mesh->MaterialIns)
							{
								SaveMaterial(mesh->MaterialPath, mesh->MaterialIns);
							}
						}
						ImGui::SliderFloat("Texture Size##textSizeMaterial", &mesh->MaterialIns->TextureSize, 0.5f, 10.f);
						ImGui::Checkbox("Transparent##transMaterial", &mesh->MaterialIns->IsTransparent);
						ImGui::ColorEdit4("Diffuse##difmaterial", glm::value_ptr(mesh->MaterialIns->Diffuse));
						ImGui::ColorEdit4("Specular##specmaterial", glm::value_ptr(mesh->MaterialIns->Specular));
						ImGui::ColorEdit4("Emissive##emismaterial", glm::value_ptr(mesh->MaterialIns->Emissive));
						ImGui::InputFloat("Ambient##ambmaterial", &mesh->MaterialIns->Ambient);
						ImGui::SliderFloat("Shininess##ambmaterial", &mesh->MaterialIns->Shininess, 0, 10.0f);
						ImGui::SliderFloat("Emissive Intensity##emintematerial", &mesh->MaterialIns->EmissiveIntensity, 0, 10.0f);

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
						if (mesh->MaterialIns->EmissiveTexture && mesh->MaterialIns->EmissiveTexture->IsLoaded())
						{
							ImGui::Image((void*)mesh->MaterialIns->EmissiveTexture->GetRendererID(), ImVec2{ 128, 128 });
							if (ImGui::Button("Unload##btnemisUNLoad"))
							{
								mesh->MaterialIns->EmissiveTexture->RemoveData();
							}
						}
						else
						{
							ImGui::InputText("Emissive Texture##emisTexmaterial", &mesh->MaterialIns->EmissiveTexturePath);
							ImGui::SameLine();
							if (ImGui::Button("Load##btnemisLoad"))
							{
								if (mesh->MaterialIns->EmissiveTexture)
								{
									mesh->MaterialIns->EmissiveTexture->CreateData(mesh->MaterialIns->EmissiveTexturePath);
								}
								else
								{
									Ref<Texture2D> texture = Texture2D::Create(mesh->MaterialIns->EmissiveTexturePath);
									if (texture->IsLoaded())
									{
										mesh->MaterialIns->EmissiveTexture = texture;
									}
								}
							}
						}

						if (ImGui::Button("Copy Component"))
						{
							//copiedMesh = *mesh;
						}
						if (ImGui::Button("Paste Component"))
						{
							//*mesh = copiedMesh;
						}
					}
				}
			}
			ImGui::EndGroup();

			// Add Component
			{
				if (ImGui::Button("Add Component"))
				{
					if (!hasLight || !hasMesh || !hasAsset)
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
								LightComponent* component = new LightComponent();
								entity.AddComponent<LightComponent>(*component);
								LightManager::AddNewLightInfo(&entity.GetComponent<LightComponent>());
								ImGui::CloseCurrentPopup();
							}
						}
						if (!hasAsset)
						{
							if (ImGui::MenuItem("Asset Component"))
							{
								addComponentCalled = false;
								entity.AddComponent<AssetComponent>(AssetComponent("",nullptr));
								ImGui::CloseCurrentPopup();
							}
						}
						if (!hasAnimator)
						{
							if (hasMesh)
							{
								if (ImGui::MenuItem("Animator Component"))
								{
									addComponentCalled = false;
									entity.AddComponent<AnimatorComponent>(AnimatorComponent());
									ImGui::CloseCurrentPopup();
								}
							}
						}
						if (!hasCC)
						{
							if (ImGui::MenuItem("Character Controller"))
							{
								addComponentCalled = false;
								CharacterControllerComponent characterController;
								physics::CharacterControllerDesc desc;
								desc.height = 2.f;
								desc.radius = 2.f;
								desc.position = entity.Transform().Translation;
								desc.rotation = entity.Transform().GetRotation();
								characterController.characterController = m_ActiveScene->GetPhysicsWorld()->CreateCharacterController(desc);
								controller = characterController.characterController;
								entity.AddComponent<CharacterControllerComponent>();
								ImGui::CloseCurrentPopup();
							}
						}
						/*if (!hasMesh)
						{
							if (ImGui::MenuItem("Mesh Component"))
							{
								addComponentCalled = false;
								entity.AddComponent<MeshComponent>(MeshComponent());
								ImGui::CloseCurrentPopup();
							}
						}*/
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

	void EditorLayer2::UI_Toolbar()
	{
	}

	void EditorLayer2::OpenProject()
	{
		ImGui::OpenPopup("Open Project");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Open Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Project Name", &m_ActiveScene->ScenePath);
			if (ImGui::Button("Load"))
			{
				m_ActiveScene->LoadScene();
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

	void EditorLayer2::SaveProject()
	{
		ImGui::OpenPopup("Save Project");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Save Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Project Name", &m_ActiveScene->ScenePath);
			if (ImGui::Button("Save"))
			{
				m_ActiveScene->SaveScene();
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

	void EditorLayer2::OnEvent(Event& event)
	{
		m_EditorCamera.OnEvent(event);
		if (event.GetEventType() == EventType::WindowResize)
		{
			auto& we = (WindowResizeEvent&)event;
			m_EditorCamera.SetViewportSize(we.GetWidth(), we.GetHeight());
		}
	}
}
