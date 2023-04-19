#include "velpch.h"
#include "Scene.h"
#include <vel/Renderer/Animation/Animator.h>
#include "Entity.h"
#include "vel/Debug/Instrumentor.h"
#include "MeshRenderer.h"
#include "MaterialSystem.h"
#include "SaveSystem.h"
#include "LightManager.h"
#include <PhysxPhysicsFactory.h>
#include "vel/Core/Application.h"
#include "vel/Renderer/Renderer.h"
#include "vel/Core/Input.h"
CRITICAL_SECTION cs_EntityMapLock;

namespace vel
{
	Scene::Scene()
		:mainAnimator(new Animator(nullptr))
	{
		entt::entity entity = m_Registry.create();
		InitializeCriticalSection(&cs_EntityMapLock);
		FrameBufferSpecification fbSpec;
		fbSpec.Width = Application::Get().GetWindow().GetWidth();
		fbSpec.Height = Application::Get().GetWindow().GetHeight();
		m_RuntimeBuffer = FrameBuffer::Create(fbSpec);

		MeshRenderer();
		MaterialSystem();
		LightManager();
		std::string maria = "assets/models/maria/Maria.fbx";
		std::string rfa = "assets/models/rfa/rfa_separate_cloth.fbx";
		std::string nightshade = "assets/models/nightshade/Nightshade J Friedrich@Idle.fbx";
		std::string medea = "assets/models/medea/medea_m_arrebola.fbx";
		std::string props = "assets/models/camp/props.fbx";
		std::string bench = "assets/models/bench/bench.fbx";
		
		/*FrameBufferSpecification fbSpec;
		fbSpec.Width = 800;
		fbSpec.Height = 600;
		m_RenderBuffer = FrameBuffer::Create(fbSpec);
		m_FBORenderBuffer = FrameBuffer::Create(fbSpec);
		m_FBOSecRenderBuffer = FrameBuffer::Create(fbSpec);
		m_SecRenderBuffer = FrameBuffer::Create(fbSpec);*/
		
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
		
		skyboxRotation = glm::vec3(0.f);
		skybox = new SkyBox(
			{
				"assets/textures/skyboxes/space/SpaceBox_right1_posX.jpg",
				"assets/textures/skyboxes/space/SpaceBox_left2_negX.jpg",
				"assets/textures/skyboxes/space/SpaceBox_top3_posY.jpg",
				"assets/textures/skyboxes/space/SpaceBox_bottom4_negY.jpg",
				"assets/textures/skyboxes/space/SpaceBox_front5_posZ.jpg",
				"assets/textures/skyboxes/space/SpaceBox_back6_negZ.jpg",
			},
			/*{
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayRight2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayLeft2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayUp2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayDown2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayFront2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayBack2048.bmp",
			},*/
			"assets/models/box.fbx"
		);
		
		/*m_ShaderLibrary.Load("assets/shaders/FBOTexture.glsl");
		m_ShaderLibrary.Get("FBOTexture")->SetInt("gAlbedoSpec", 0);*/
		

		m_PhysicsFactory = new physics::physxim::PhysicsFactory();
		m_PhysicsWorld = m_PhysicsFactory->CreateWorld();
		m_Shader = m_ShaderLibrary.Load("assets/shaders/gBuffer.glsl");
		m_Shader->Bind();
		m_Shader->SetInt("u_TextureDiffuse", 0);
		m_Shader->SetInt("u_TextureNormal", 1);
		m_Shader->SetInt("u_TextureSpecular", 2);
		m_Shader->SetInt("u_TextureEmissive", 3);
		m_Shader->SetInt("skyBox", 4);
		
		m_SkyBoxShader = m_ShaderLibrary.Load("assets/shaders/SkyBox.glsl");
		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetInt("skybox", 0);
		//-1.7939719, 28.011202, -68.473145
		ScenePath = "BattleScene";
		LoadScene();
	}
	Scene::~Scene()
	{
		DeleteCriticalSection(&cs_EntityMapLock);
	}
	void Scene::ReloadShader()
	{
		m_Shader = m_ShaderLibrary.Reload("assets/shaders/gBuffer.glsl","gBuffer");
		m_Shader->Bind();
		m_Shader->SetInt("u_TextureDiffuse", 0);
		m_Shader->SetInt("u_TextureNormal", 1);
		m_Shader->SetInt("u_TextureSpecular", 2);
		m_Shader->SetInt("u_TextureEmissive", 3);
		m_Shader->SetInt("skyBox", 4);
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Update Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateFunction();
						nsc.Instance->m_Entity = Entity{ entity, this };
						nsc.Instance->OnCreate();
					}
					nsc.Instance->OnUpdate(ts);
				});
		}

		m_PhysicsWorld->TimeStep(1 / 60.f);
		glm::vec4 eyeLocation = glm::vec4(1.f);
		if(!mainCamera)
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto cameraEntity : view)
			{
				auto [transformcomp, camera] = view.get<TransformComponent, CameraComponent>(cameraEntity);
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					break;
				}
			}
		}
		if (mainCamera)
		{
			/*m_RuntimeBuffer->Bind();
			RenderCommand::SetClearColor(glm::vec4(0.f, 0.f, 0.f, 1));
			RenderCommand::Clear();
			RenderCommand::EnableDepth();*/
			eyeLocation = glm::vec4(mainCamera->Position.x,mainCamera->Position.y, mainCamera->Position.z, 1.f);
			Renderer::BeginScene(*mainCamera, glm::mat4(1.f));
			{
				auto lightView = m_Registry.group<LightComponent>(entt::get<TransformComponent>);
				for (auto lights : lightView)
				{
					auto [transform, light] = lightView.get<TransformComponent, LightComponent>(lights);
					Entity entity = Entity(lights, this);
					if (entity.enabled)
					{
						glm::vec3 pos, sca;
						glm::quat rot;
						Math::DecomposeTransform(GetWorldSpaceTransformMatrix(entity), pos, rot, sca);
						light.Position = glm::vec4(pos, 1.0);
						light.Direction = glm::vec4(glm::eulerAngles(rot), 1.0);
					}
				}
			}
			{
				auto ccView = m_Registry.group<CharacterControllerComponent>(entt::get<TransformComponent>);
				for (auto ent : ccView)
				{
					auto [transform, characterController] = ccView.get<TransformComponent, CharacterControllerComponent>(ent);

					// TODO Check out the cc offset
					if(characterController.characterController)
						transform.Translation = characterController.characterController->GetPosition() + glm::vec3(0.f, 1.f, 0.f);
				}
			}
			{
				auto rigidView = m_Registry.group<RigidbodyComponent>(entt::get<TransformComponent>);
				for (auto ent : rigidView)
				{
					auto [transform, rigidComp] = rigidView.get<TransformComponent, RigidbodyComponent>(ent);
					if (rigidComp.desc.IsKinematic)
					{
						if(rigidComp.rigidBody)
							rigidComp.rigidBody->SetPosition(transform.Translation);
					}
					else
					{
						if (rigidComp.rigidBody)
						{
							float y = transform.Translation.y;
							if (rigidComp.rigidBody)
								rigidComp.rigidBody->GetPosition(transform.Translation);
						}
					}
				}
			}
			{
				auto view = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
				int l = view.size();
				std::vector<entt::entity> transparentEntities;
				for (auto e : view)
				{
					Entity entity = Entity(e, this);
					auto [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(e);
					glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
					if (IsEnabled(entity))
					{
						Ref<Material> MaterialIns = MaterialSystem::GetMaterial(meshComponent.MaterialPath);
						if (!MaterialIns)
							MaterialIns = meshComponent.MaterialIns;

						if (MaterialIns && !MaterialIns->IsCompiled)
						{
							if (MaterialIns && !MaterialIns->DiffuseTexturePath.empty())
							{
								Ref<Texture2D> diffTex = Texture2D::Create(MaterialIns->DiffuseTexturePath);
								if (diffTex != nullptr)
								{
									MaterialIns->DiffuseTexture = diffTex;
								}
							}
							if (MaterialIns && !MaterialIns->NormalTexturePath.empty())
							{
								Ref<Texture2D> normTex = Texture2D::Create(MaterialIns->NormalTexturePath);
								if (normTex != nullptr)
								{
									MaterialIns->NormalTexture = normTex;
								}
							}
							if (MaterialIns && !MaterialIns->SpecularTexturePath.empty())
							{
								Ref<Texture2D> specTex = Texture2D::Create(MaterialIns->SpecularTexturePath);
								if (specTex != nullptr)
								{
									MaterialIns->SpecularTexture = specTex;
								}
							}
							if (MaterialIns && !MaterialIns->EmissiveTexturePath.empty())
							{
								Ref<Texture2D> emisTex = Texture2D::Create(MaterialIns->EmissiveTexturePath);
								if (emisTex != nullptr)
								{
									MaterialIns->EmissiveTexture = emisTex;
								}
							}
							MaterialIns->IsCompiled = true;
						}
						if (MaterialIns)
						{
							if (MaterialIns->IsTransparent)
							{
								AddTransparentEntiy(&transparentEntities, e);
								continue;
							}
							m_Shader->Bind();
							BindSkyBox(4);
							m_Shader->SetFloat4("eyeLocation", eyeLocation);
							m_Shader->SetFloat("u_texsize", MaterialIns->TextureSize);
							meshComponent.shader = m_Shader;
							if (meshComponent.MeshDrawData && meshComponent.MeshDrawData->m_UseBones)
							{
								if (entity.HasComponent<AnimatorComponent>() && entity.GetComponent<AnimatorComponent>().UseAnimation)
								{
									AnimatorComponent& animator = entity.GetComponent<AnimatorComponent>();
									if (animator.animator)
									{
										animator.animator->UpdateAnimation(ts);
										std::vector<glm::mat4>& transforms = animator.animator->GetFinalBoneMatrices();
										//m_Shader->SetMat4("u_RotationMatrix", glm::mat4(1.f) * glm::toMat4(transformComponent.GetRotation()));
										for (int i = 0; i < transforms.size(); ++i)
										{
											m_Shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

											glm::vec3 pos, sca;
											glm::quat rotation;
											Math::DecomposeTransform(transform, pos, rotation, sca);
											m_Shader->SetMat4("u_RotationMatrix[" + std::to_string(i) + "]", glm::mat4(1.f) * glm::toMat4(rotation));
										}
									}
								}
								else
								{
									for (int i = 0; i < 100; ++i)
									{
										m_Shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.f));

										glm::vec3 pos, sca;
										glm::quat rotation;
										Math::DecomposeTransform(transform, pos, rotation, sca);
										m_Shader->SetMat4("u_RotationMatrix[" + std::to_string(i) + "]", glm::mat4(1.f) * glm::toMat4(rotation));
									}
								}
							}
							MeshRenderer::DrawMesh(meshComponent, transform);
						}
					}
				}

				for (entt::entity handle : transparentEntities)
				{
					Entity entity = Entity(handle, this);
					MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();
					Ref<Material> MaterialIns = MaterialSystem::GetMaterial(meshComponent.MaterialPath);
					glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
					m_Shader->Bind();
					BindSkyBox(4);
					m_Shader->SetFloat4("eyeLocation", eyeLocation);
					m_Shader->SetFloat("u_texsize", MaterialIns->TextureSize);
					meshComponent.shader = m_Shader;
					MeshRenderer::DrawMesh(meshComponent, transform);
				}
			}
			/*Renderer::EndScene();
			m_RuntimeBuffer->Unbind();*/
		}
	}
	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer::BeginScene(camera.GetViewMatrix(), camera.GetUnReversedProjectionMatrix());
		// Lights
		mainAnimator->UpdateAnimation(ts);
		m_PhysicsWorld->TimeStep(1 / 60.f);
		{
			auto lightView = m_Registry.group<LightComponent>(entt::get<TransformComponent>);
			for (auto lights : lightView)
			{
				auto [transform, light] = lightView.get<TransformComponent, LightComponent>(lights);
				Entity entity = Entity(lights, this);
				if (entity.enabled)
				{
					glm::vec3 pos, sca;
					glm::quat rot;
					Math::DecomposeTransform(GetWorldSpaceTransformMatrix(entity), pos, rot, sca);
					light.Position = glm::vec4(pos, 1.0);
					light.Direction = glm::vec4(glm::eulerAngles(rot), 1.0);
				}
			}
		}
		{
			auto ccView = m_Registry.group<CharacterControllerComponent>(entt::get<TransformComponent>);
			for (auto ent : ccView)
			{
				auto [transform, characterController] = ccView.get<TransformComponent, CharacterControllerComponent>(ent);
				// TODO Check out the cc offset
				if(characterController.characterController)
					transform.Translation = characterController.characterController->GetPosition() + glm::vec3(0.f, 1.f, 0.f);
			}
		}
		{
			auto rigidView = m_Registry.group<RigidbodyComponent>(entt::get<TransformComponent>);
			for (auto ent : rigidView)
			{
				auto [transform, rigidComp] = rigidView.get<TransformComponent, RigidbodyComponent>(ent);
				if (rigidComp.desc.IsKinematic)
				{
					if (rigidComp.rigidBody)
						rigidComp.rigidBody->SetPosition(transform.Translation);
				}
				else
				{
					float y = transform.Translation.y;
					if (rigidComp.rigidBody)
						rigidComp.rigidBody->GetPosition(transform.Translation);
					//transform.Translation.y = y;
				}
			}
		}
		{
			auto view = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
			int l = view.size();
			std::vector<entt::entity> transparentEntities;
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				auto [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(e);
				glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
				if (IsEnabled(entity))
				{
					Ref<Material> MaterialIns = MaterialSystem::GetMaterial(meshComponent.MaterialPath);
					if(!MaterialIns)
						MaterialIns = meshComponent.MaterialIns;

					if (MaterialIns && !MaterialIns->IsCompiled)
					{
						if (MaterialIns && !MaterialIns->DiffuseTexturePath.empty())
						{
							Ref<Texture2D> diffTex = Texture2D::Create(MaterialIns->DiffuseTexturePath);
							if (diffTex != nullptr)
							{
								MaterialIns->DiffuseTexture = diffTex;
							}
						}
						if (MaterialIns && !MaterialIns->NormalTexturePath.empty())
						{
							Ref<Texture2D> normTex = Texture2D::Create(MaterialIns->NormalTexturePath);
							if (normTex != nullptr)
							{
								MaterialIns->NormalTexture = normTex;
							}
						}
						if (MaterialIns && !MaterialIns->SpecularTexturePath.empty())
						{
							Ref<Texture2D> specTex = Texture2D::Create(MaterialIns->SpecularTexturePath);
							if (specTex != nullptr)
							{
								MaterialIns->SpecularTexture = specTex;
							}
						}
						if (MaterialIns && !MaterialIns->EmissiveTexturePath.empty())
						{
							Ref<Texture2D> emisTex = Texture2D::Create(MaterialIns->EmissiveTexturePath);
							if (emisTex != nullptr)
							{
								MaterialIns->EmissiveTexture = emisTex;
							}
						}
						MaterialIns->IsCompiled = true;
					}
					if (MaterialIns)
					{
						if (MaterialIns->IsTransparent)
						{
							AddTransparentEntiy(&transparentEntities, e);
							continue;
						}
						m_Shader->Bind();
						BindSkyBox(4);
						m_Shader->SetFloat4("eyeLocation", { camera.GetPosition().x,camera.GetPosition().y, camera.GetPosition().z, 1.f });
						m_Shader->SetFloat("u_texsize", MaterialIns->TextureSize);
						meshComponent.shader = m_Shader;
						if (meshComponent.MeshDrawData && meshComponent.MeshDrawData->m_UseBones)
						{
							if (entity.HasComponent<AnimatorComponent>() && entity.GetComponent<AnimatorComponent>().UseAnimation)
							{
								AnimatorComponent& animator = entity.GetComponent<AnimatorComponent>();
								if (animator.animator)
								{
									animator.animator->UpdateAnimation(ts);
									std::vector<glm::mat4>& transforms = animator.animator->GetFinalBoneMatrices();
									//m_Shader->SetMat4("u_RotationMatrix", glm::mat4(1.f) * glm::toMat4(transformComponent.GetRotation()));
									for (int i = 0; i < transforms.size(); ++i)
									{
										m_Shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

										glm::vec3 pos, sca;
										glm::quat rotation;
										Math::DecomposeTransform(transform, pos, rotation, sca);
										m_Shader->SetMat4("u_RotationMatrix[" + std::to_string(i) + "]", glm::mat4(1.f) * glm::toMat4(rotation));
									}
								}
							}
							else
							{
								for (int i = 0; i < 200; ++i)
								{
									m_Shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.f));

									glm::vec3 pos, sca;
									glm::quat rotation;
									Math::DecomposeTransform(transform, pos, rotation, sca);
									m_Shader->SetMat4("u_RotationMatrix[" + std::to_string(i) + "]", glm::mat4(1.f) * glm::toMat4(rotation));
								}
							}
						}
						MeshRenderer::DrawMesh(meshComponent, transform);
					}
				}
			}

			for (entt::entity handle : transparentEntities)
			{
				Entity entity = Entity(handle, this);
				MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();
				Ref<Material> MaterialIns = MaterialSystem::GetMaterial(meshComponent.MaterialPath);
				glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
				m_Shader->Bind();
				BindSkyBox(4);
				m_Shader->SetFloat4("eyeLocation", {camera.GetPosition().x,camera.GetPosition().y, camera.GetPosition().z, 1.f});
				m_Shader->SetFloat("u_texsize", MaterialIns->TextureSize);
				meshComponent.shader = m_Shader;
				MeshRenderer::DrawMesh(meshComponent, transform);
			}
		}


		Renderer::EndScene();
	}
	void Scene::OnEvent(Event& e)
	{
	}
	Entity Scene::CreateEntity(const std::string& name, EntityType type)
	{
		return CreateChildEntity({}, name, type);
	}
	Entity Scene::CreateChildEntity(Entity parent, const std::string& name, EntityType type)
	{
		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = {};
		idComponent.Type = type;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);
		else
			entity.AddComponent<TagComponent>(Entity::NoName);

		entity.AddComponent<RelationshipComponent>();

		if (parent)
			entity.SetParent(parent);

		EnterCriticalSection(&cs_EntityMapLock);
		m_EntityIDMap.emplace(idComponent.ID,entity);
		LeaveCriticalSection(&cs_EntityMapLock);
		//SortEntities();

		return entity;
	}
	Entity Scene::CreateEntityWithID(GUID guid, const std::string& name, bool shouldSort, EntityType type)
	{
		VEL_PROFILE_FUNCTION();

		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = guid;
		idComponent.Type = type;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);
		else 
			entity.AddComponent<TagComponent>(Entity::NoName);
		
		entity.AddComponent<RelationshipComponent>();

		VEL_CORE_ASSERT(m_EntityIDMap.find(guid) == m_EntityIDMap.end(),"");


		EnterCriticalSection(&cs_EntityMapLock);
		m_EntityIDMap[guid] = entity;
		LeaveCriticalSection(&cs_EntityMapLock);

		if (shouldSort)
			SortEntities();

		return entity;
	}
	Entity Scene::CreateChildEntityWithID(GUID guid, Entity parent, const std::string& name, bool shouldSort, EntityType type)
	{
		VEL_PROFILE_FUNCTION();

		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = guid;
		idComponent.Type = type;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);
		else
			entity.AddComponent<TagComponent>(Entity::NoName);

		entity.AddComponent<RelationshipComponent>();

		if (parent)
			entity.SetParent(parent);

		//VEL_CORE_ASSERT(m_EntityIDMap.find(guid) == m_EntityIDMap.end(), "");


		EnterCriticalSection(&cs_EntityMapLock);
		m_EntityIDMap[guid] = entity;
		LeaveCriticalSection(&cs_EntityMapLock);

		if (shouldSort)
			SortEntities();

		return entity;
	}
	Entity Scene::CreateAssetEntityWithID(GUID guid, Entity parent, const std::string& name, bool shouldSort, EntityType type)
	{
		VEL_PROFILE_FUNCTION();

		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = {};
		idComponent.AssetID = guid;
		idComponent.Type = type;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);
		else
			entity.AddComponent<TagComponent>(Entity::NoName);

		entity.AddComponent<RelationshipComponent>();

		if (parent)
			entity.SetParent(parent);

		VEL_CORE_ASSERT(m_EntityIDMap.find(idComponent.ID) == m_EntityIDMap.end(), "");


		EnterCriticalSection(&cs_EntityMapLock);
		m_EntityIDMap.emplace(idComponent.ID,entity);
		LeaveCriticalSection(&cs_EntityMapLock);

		if (shouldSort)
			SortEntities();

		return entity;
	}
	Entity Scene::DuplicateEntity(Entity& entity, Entity& parentEntity)
	{
		IDComponent& idComponent = entity.GetComponent<IDComponent>();
		TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
		Entity newEntity;
		std::string materialLocation = "";
		if (parentEntity)
		{
			newEntity = CreateChildEntity(parentEntity, entity.Name() + "_Duplicate", idComponent.Type);
		}
		else
		{
			newEntity = CreateEntity(entity.Name() + "_Duplicate", idComponent.Type);
		}

		// Asset Component
		{
			if (entity.HasComponent<AssetComponent>())
			{
				AssetComponent& assetComponent = entity.GetComponent<AssetComponent>();
				newEntity.AddComponent<AssetComponent>(AssetComponent(assetComponent.FileLocation, assetComponent.ModelInstance));
			}
		}

		// Transform Component
		{
			TransformComponent* newTransform = &newEntity.Transform();
			TransformComponent* transform = &entity.Transform();
			newTransform->RotationEuler = transform->RotationEuler;
			newTransform->Translation = transform->Translation;
			newTransform->Scale = transform->Scale;
			newTransform->enabled = transform->enabled;
		}

		// Rigidbody Component
		{
			if (entity.HasComponent<RigidbodyComponent>())
			{
				RigidbodyComponent& rigidBody = entity.GetComponent<RigidbodyComponent>();
				newEntity.AddComponent<RigidbodyComponent>(RigidbodyComponent(rigidBody));
			}
		}

		// Mesh Component
		{
			if (entity.HasComponent<MeshComponent>())
			{
				MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();
				materialLocation = meshComponent.MaterialPath;
				newEntity.AddComponent<MeshComponent>(MeshComponent(meshComponent));
			}
		}

		// Animator Component
		{
			if (entity.HasComponent<AnimatorComponent>())
			{
				AnimatorComponent& animatorComponent = entity.GetComponent<AnimatorComponent>();
				newEntity.AddComponent<AnimatorComponent>(AnimatorComponent(animatorComponent));
			}
		}

		// Light Component
		{
			if (entity.HasComponent<LightComponent>())
			{
				LightComponent& lightComponent = entity.GetComponent<LightComponent>();
				newEntity.AddComponent<LightComponent>(LightComponent(lightComponent));
			}
		}

		if (idComponent.Type == EntityType::IsAsset)
		{
			IDComponent& newIdComponent = newEntity.GetComponent<IDComponent>();
			newIdComponent.AssetID = idComponent.AssetID;
			newIdComponent.Type = EntityType::IsAsset;
			bool loop = true;
			Entity parentWithAsset;
			Entity currentEntity = newEntity;
			while (loop)
			{
				parentWithAsset = currentEntity.GetParent();
				if (!parentWithAsset)
					loop = false;
				parentWithAsset.SetScene(this);
				if (parentWithAsset.HasComponent<AssetComponent>())
					loop = false;
				currentEntity = parentWithAsset;
			}
			if (parentWithAsset && parentWithAsset.HasComponent<AssetComponent>())
			{
				VEL_CORE_INFO("Adding new asset");
				AssetComponent& asset = parentWithAsset.GetComponent<AssetComponent>();
				asset.AssetHandle.push_back({ newEntity.Name(),newEntity.GetGUID(),newEntity.GetAssetID(), materialLocation });
			}
		}

		for (GUID childEntityGUID : entity.Children())
		{
			DuplicateEntity(GetEntityWithGUID(childEntityGUID), newEntity);
		}
		return newEntity;
	}
	void Scene::DeleteEntity(Entity& entity)
	{
		if (entity.Children().size() > 0)
		{
			for (GUID child : entity.Children())
			{
				Entity childEntity = TryGetEntityWithGUID(child);
				if(childEntity)
					DeleteEntity(childEntity);
			}
		}
		m_Registry.destroy(entity.m_EntityHandle);
	}
	Entity Scene::GetEntityWithGUID(GUID id) const
	{
		VEL_PROFILE_FUNCTION();
		VEL_CORE_ASSERT(m_EntityIDMap.find(id) != m_EntityIDMap.end(), "Invalid entity ID or entity doesn't exist in scene!");
		return m_EntityIDMap.at(id);
	}
	Entity Scene::GetAssetEntityWithGUID(GUID id) const
	{

		VEL_CORE_ASSERT(m_EntityIDMap.find(id) != m_EntityIDMap.end(), "Invalid entity ID or entity doesn't exist in scene!");
		return m_EntityIDMap.at(id);
	}
	Entity Scene::TryGetEntityWithGUID(GUID id) const
	{
		if (const auto iter = m_EntityIDMap.find(id); iter != m_EntityIDMap.end())
			return iter->second;
		return Entity{};
	}
	Entity Scene::TryGetEntityWithTag(const std::string& tag)
	{
		VEL_PROFILE_FUNCTION();
		auto entities = GetAllEntitiesWith<TagComponent>();
		for (auto e : entities)
		{
			if (entities.get<TagComponent>(e).Tag == tag)
				return Entity(e, const_cast<Scene*>(this));
		}

		return Entity{};
	}
	Entity Scene::TryGetDescendantEntityWithTag(Entity entity, const std::string& tag)
	{
		return Entity();
	}
	void Scene::ParentEntity(Entity entity, Entity parent)
	{
		VEL_PROFILE_FUNCTION();

		if (parent.IsDescendantOf(entity))
		{
			UnparentEntity(parent);

			Entity newParent = TryGetEntityWithGUID(entity.GetParentGUID());
			if (newParent)
			{
				UnparentEntity(entity);
				ParentEntity(parent, newParent);
			}
		}
		else
		{
			Entity previousParent = TryGetEntityWithGUID(entity.GetParentGUID());

			if (previousParent)
				UnparentEntity(entity);
		}

		entity.SetParentGUID(parent.GetGUID());
		parent.Children().push_back(entity.GetGUID());

		ConvertToLocalSpace(entity);
	}

	void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
	{
		VEL_PROFILE_FUNCTION();

		Entity parent = TryGetEntityWithGUID(entity.GetParentGUID());
		if (!parent)
			return;

		auto& parentChildren = parent.Children();
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetGUID()), parentChildren.end());

		if (convertToWorldSpace)
			ConvertToWorldSpace(entity);

		entity.SetParentGUID(0);
	}

	void Scene::ConvertToLocalSpace(Entity entity)
	{
		VEL_PROFILE_FUNCTION();

		Entity parent = TryGetEntityWithGUID(entity.GetParentGUID());

		if (!parent)
			return;

		auto& transform = entity.Transform();
		glm::mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);
		glm::mat4 localTransform = glm::inverse(parentTransform) * transform.GetTransform();
		transform.SetTransform(localTransform);
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
		VEL_PROFILE_FUNCTION();

		Entity parent = TryGetEntityWithGUID(entity.GetParentGUID());

		if (!parent)
			return;

		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		auto& entityTransform = entity.Transform();
		entityTransform.SetTransform(transform);
	}

	glm::mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
	{
		glm::mat4 transform(1.0f);

		Entity parent = TryGetEntityWithGUID(entity.GetParentGUID());
		//parent.SetScene(this);
		//std::string parentName = parent.Name();
		if (parent)
			transform = GetWorldSpaceTransformMatrix(parent);

		return transform * entity.Transform().GetTransform();
	}

	TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
	{
		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		TransformComponent transformComponent;
		transformComponent.SetTransform(transform);
		return transformComponent;
	}

	glm::vec3 Scene::GetParentScale(Entity entity)
	{
		glm::vec3 scale = entity.Transform().Scale;
		if (entity.GetParent())
		{
			scale = GetParentScale(entity.GetParent());
		}
		return scale;
	}

	void Scene::BindSkyBox(int i)
	{
		skybox->skyboxTexture->Bind(i);
	}

	void Scene::LoadScene()
	{	
		m_Registry.clear();
		const std::string path = "assets/scenes/" + ScenePath + ".xml";
		VEL_CORE_INFO("Loading Scene: {0}", path);
		LightManager::Clear();
		LoadSceneFile(path, this, true);
	}

	void Scene::SaveScene()
	{
		const std::string path = "assets/scenes/" + ScenePath + ".xml";
		VEL_INFO("Saving Scene: {0}", path);
		SaveSceneFile(path, this);
	}

	void Scene::DrawSkyBox(glm::mat4 viewProjection)
	{
		//step += 0.0002f;
		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetFloat("step", step);
		skybox->skyboxTexture->DrawSkyBox(viewProjection * glm::toMat4(glm::quat(skyboxRotation)), m_SkyBoxShader, skybox->cubeModel->GetMeshData().m_VertexArray);
	}

	void Scene::BindLightData(Ref<Shader> shader, glm::vec4 eyepos)
	{
		shader->SetFloat4("eyeLocation", eyepos);
		LightManager::CopyLightInformationToShader(shader);
	}

	void Scene::LoadAnimation(Animation* animation)
	{
		mainAnimator = new Animator(animation);
	}

	void Scene::SortEntities()
	{
		m_Registry.sort<IDComponent>([&](const auto lhs, const auto rhs)
		{
			auto lhsEntity = m_EntityIDMap.find(lhs.ID);
			auto rhsEntity = m_EntityIDMap.find(rhs.ID);
			return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
		});
	}

	void Scene::AddTransparentEntiy(std::vector<entt::entity>* list, entt::entity entity)
	{
		list->push_back(entity);
	}

	bool Scene::IsEnabled(Entity entity)
	{
		bool enabled = entity.Transform().enabled;
		if (!enabled)
			return false;
		Entity parent = TryGetEntityWithGUID(entity.GetParentGUID());
		if (parent)
		{
			enabled = IsEnabled(parent);
		}
		return enabled;
	}
	void  Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewPortSize = { width, height };
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto cameraEntity : view)
		{
			auto& [transformcomp, camera] = view.get<TransformComponent, CameraComponent>(cameraEntity);
			camera.Camera.SetViewportSize(width, height);
		}
	}
}