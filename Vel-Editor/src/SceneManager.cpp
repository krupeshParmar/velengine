#include "SceneManager.h"
CRITICAL_SECTION SceneManagerEntitesList_Lock;

namespace vel
{
	SceneManager::SceneManager()
	{
		InitializeCriticalSection(&SceneManagerEntitesList_Lock);
		MeshRenderer();
		MaterialSystem();
		std::string maria = "assets/models/maria/Maria.fbx";
		std::string rfa = "assets/models/rfa/rfa_separate_cloth.fbx";
		std::string nightshade = "assets/models/nightshade/Nightshade J Friedrich@Idle.fbx";
		std::string medea = "assets/models/medea/medea_m_arrebola.fbx";
		std::string props = "assets/models/camp/props.fbx";
		std::string bench = "assets/models/bench/bench.fbx";

		m_EntityManager = CreateRef<EntityManager>();
		m_TempEntityManager = CreateRef<EntityManager>();

		FrameBufferSpecification fbSpec;
		fbSpec.Width = 800;
		fbSpec.Height = 600;
		m_RenderBuffer = FrameBuffer::Create(fbSpec);
		m_FBORenderBuffer = FrameBuffer::Create(fbSpec);

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

		/*uint32_t id = m_EntityManager->CreateEntity("First GameObject");

		MeshComponent* mesh1 = new MeshComponent(maria, false);

		m_EntityManager->AddComponent(id, mesh1);

		uint32_t lightBulb = m_EntityManager->CreateEntity("Directional Light");
		m_EntityManager->AddComponent(lightBulb, new LightComponent());
		m_EntityManager->AddComponent(lightBulb, new MeshComponent(std::string("assets/models/debug_sphere.fbx"), true));
*/

		/*
		* {
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayRight2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayLeft2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayUp2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayDown2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayFront2048.bmp",
				"assets/textures/skyboxes/sunnyday/TropicalSunnyDayBack2048.bmp",
			}
		*/
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

		m_Shader = m_ShaderLibrary.Load("assets/shaders/gBuffer.glsl");
		m_TempShader = m_ShaderLibrary.Load("temp", "assets/shaders/simpleshader1.glsl");
		m_ShaderLibrary.Load("assets/shaders/FBOTexture.glsl");
		m_ShaderLibrary.Get("FBOTexture")->SetInt("gAlbedoSpec", 0);

		m_TempShader->Bind();
		m_TempShader->SetInt("u_TextureDiffuse", 0);
		m_TempShader->SetInt("u_TextureNormal", 1);
		m_TempShader->SetInt("u_TextureSpecular", 2);
		m_TempShader->SetInt("skyBox", 3);

		m_Shader->Bind();
		m_Shader->SetInt("u_TextureDiffuse", 0);
		m_Shader->SetInt("u_TextureNormal", 1);
		m_Shader->SetInt("u_TextureSpecular", 2);
		m_Shader->SetInt("skyBox", 3);


		m_SkyBoxShader = m_ShaderLibrary.Load("assets/shaders/SkyBox.glsl");
		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetInt("skybox", 0);
		//-1.7939719, 28.011202, -68.473145
		ScenePath = "MariaTest";
		LoadSceneFile("assets/scenes/OffScreen1.xml", m_TempEntityManager, true);
		LoadScene();
	}
	SceneManager::~SceneManager()
	{
		DeleteCriticalSection(&SceneManagerEntitesList_Lock);
	}
	void SceneManager::CreateNewScene()
	{

	}
	void SceneManager::Update(Ref<FrameBuffer> buffer, float dt, glm::vec4 eyeLocation)
	{
		std::vector<Ref<Entity>> entities = m_EntityManager->GetAllEntities();
		for (std::vector<Ref<Entity>>::iterator entityit = entities.begin();
			entityit != entities.end();
			entityit++)
		{
			Ref<Entity> entity = *entityit;
			if (!entity || !entity->enabled)
				continue;
			TransformComponent* transform = m_EntityManager->GetComponentByType<TransformComponent>(entity->GetID());			
			if (m_EntityManager->HasComponent<LightComponent>(entity->GetID()))
			{
				LightComponent* lightComp = m_EntityManager->GetComponentByType<LightComponent>(entity->GetID());
				if (lightComp->Enabled)
				{
					lightComp->Position = glm::vec4(transform->Translation, 1.0);
					lightComp->Direction = glm::vec4(transform->GetRotation(), 1.0);
				}
			}
			
			if (m_EntityManager->HasComponent<MeshComponent>(entity->GetID()))
			{
				MeshComponent* meshComp = m_EntityManager->GetComponentByType<MeshComponent>(entity->GetID());
				if (meshComp->Enabled)
				{
					if (meshComp->MaterialIns && !meshComp->MaterialIns->IsCompiled)
					{
						if (!meshComp->MaterialIns->DiffuseTexturePath.empty())
						{
							Ref<Texture2D> diffTex = Texture2D::Create(meshComp->MaterialIns->DiffuseTexturePath);
							if (diffTex != nullptr)
							{
								meshComp->MaterialIns->DiffuseTexture = diffTex;
							}
						}
						if (meshComp->MaterialIns && !meshComp->MaterialIns->NormalTexturePath.empty())
						{
							Ref<Texture2D> normTex = Texture2D::Create(meshComp->MaterialIns->NormalTexturePath);
							if (normTex != nullptr)
							{
								meshComp->MaterialIns->NormalTexture = normTex;
							}
						}
						if (meshComp->MaterialIns && !meshComp->MaterialIns->SpecularTexturePath.empty())
						{
							Ref<Texture2D> specTex = Texture2D::Create(meshComp->MaterialIns->SpecularTexturePath);
							if (specTex != nullptr)
							{
								meshComp->MaterialIns->SpecularTexture = specTex;
							}
						}
						meshComp->MaterialIns->IsCompiled = true;
					}
					if (meshComp->ModelIns && meshComp->MaterialIns)
					{
						m_Shader->Bind();
						BindSkyBox(3);
						m_Shader->SetFloat4("eyeLocation", eyeLocation);
						if (entity->name == "Plane")
							meshComp->MaterialIns->TextureSize = 10.f;

						m_Shader->SetFloat("u_texsize", meshComp->MaterialIns->TextureSize);

						if (entity->name == "BedroomQuad")
						{
							ToBeDrawn = entity;
							//meshComp->ModelIns->DrawMesh(m_FBORenderBuffer, buffer, m_Shader, meshComp->MaterialIns, transform->GetTransform());
						}
						else meshComp->ModelIns->DrawMesh(m_Shader, meshComp->MaterialIns, transform->GetTransform());
					}
				}
			}
		}
	}
	void SceneManager::BindSkyBox(int i)
	{
		skybox->skyboxTexture->Bind(i);
	}
	void SceneManager::LoadScene()
	{
		const std::string path = "assets/scenes/" + ScenePath + ".xml";
		VEL_INFO("Loading Scene: {0}", path);
		LoadSceneFile(path, m_EntityManager, true);
	}
	void SceneManager::SaveScene()
	{
		const std::string path = "assets/scenes/" + ScenePath + ".xml";
		VEL_INFO("Saving Scene: {0}", path);
		if (!SaveSceneFile(path, m_EntityManager))
		{
			int breakme = 0;
		}
	}

	void SceneManager::DrawSkyBox(glm::mat4 viewProjection)
	{
		skybox->skyboxTexture->DrawSkyBox(viewProjection, m_SkyBoxShader, skybox->cubeModel->GetMeshData().m_VertexArray);
	}
	void SceneManager::BindLightData(Ref<Shader> shader, glm::vec4 eyepos)
	{
		LightManager::CopyLightInformationToShader(shader);
		shader->SetFloat4("eyeLocation", eyepos);

	}

	// This is a MESS
	void SceneManager::DrawBedroom(Ref<FrameBuffer> buffer, glm::vec4 eyeLocation)
	{
		buffer->Bind();
		RenderCommand::SetClearColor(glm::vec4(1.f));
		RenderCommand::EnableDepth();
		RenderCommand::Clear();
		glm::mat4 matView = glm::lookAt(
			glm::vec3(eyeLocation.x, eyeLocation.y, eyeLocation.z),
			glm::vec3(0.f,30.f,0.f),
			glm::vec3(0.f, 1.f, 0.f)
		);

		glm::mat4 matProjection = glm::perspective(
			45.0f,
			1280.f / 720.f,
			1.f,
			40.0f);
		Renderer::BeginScene(matView, matProjection);
		std::vector<Ref<Entity>> entities = m_TempEntityManager->GetAllEntities();
		for (std::vector<Ref<Entity>>::iterator entityit = entities.begin();
			entityit != entities.end();
			entityit++)
		{
			Ref<Entity> entity = *entityit;
			if (!entity || !entity->enabled)
				continue;
			TransformComponent* transform = m_TempEntityManager->GetComponentByType<TransformComponent>(entity->GetID());
			if(entity->name == "Maria")
				transform->Rotation.y += 0.1f;
			if (m_TempEntityManager->HasComponent<LightComponent>(entity->GetID()))
			{
				LightComponent* lightComp = m_TempEntityManager->GetComponentByType<LightComponent>(entity->GetID());
				if (lightComp->Enabled)
				{
					lightComp->Position = glm::vec4(transform->Translation, 1.0);
					lightComp->Direction = glm::vec4(transform->GetRotation(), 1.0);
				}
			}

			if (m_TempEntityManager->HasComponent<MeshComponent>(entity->GetID()))
			{
				MeshComponent* meshComp = m_TempEntityManager->GetComponentByType<MeshComponent>(entity->GetID());
				
				if (meshComp->Enabled)
				{
					if (meshComp->MaterialIns && !meshComp->MaterialIns->IsCompiled)
					{
						if (!meshComp->MaterialIns->DiffuseTexturePath.empty())
						{
							Ref<Texture2D> diffTex = Texture2D::Create(meshComp->MaterialIns->DiffuseTexturePath);
							if (diffTex != nullptr)
							{
								meshComp->MaterialIns->DiffuseTexture = diffTex;
							}
						}
						if (meshComp->MaterialIns && !meshComp->MaterialIns->NormalTexturePath.empty())
						{
							Ref<Texture2D> normTex = Texture2D::Create(meshComp->MaterialIns->NormalTexturePath);
							if (normTex != nullptr)
							{
								meshComp->MaterialIns->NormalTexture = normTex;
							}
						}
						if (meshComp->MaterialIns && !meshComp->MaterialIns->SpecularTexturePath.empty())
						{
							Ref<Texture2D> specTex = Texture2D::Create(meshComp->MaterialIns->SpecularTexturePath);
							if (specTex != nullptr)
							{
								meshComp->MaterialIns->SpecularTexture = specTex;
							}
						}
						meshComp->MaterialIns->IsCompiled = true;
					}
					if (meshComp->ModelIns)
					{
						std::vector<Ref<MeshData>> meshes = meshComp->ModelIns->GetWholeMesh();
						Ref<Material> material = meshComp->MaterialIns;
						m_TempShader->Bind();
						m_TempShader->SetFloat4("eyeLocation", eyeLocation);
						if (meshes.empty())
							continue;
						for (Ref<MeshData> meshData : meshes)
						{
							if (meshData->m_VertexArray == nullptr)
							{
								if (meshData->m_Loaded)
								{
									meshData->m_VertexArray = VertexArray::Create();
									vel::Ref<vel::VertexBuffer> vertexBuffer;
									vertexBuffer = vel::VertexBuffer::Create(
										&meshData->m_Vertices[0],
										sizeof(vel::Vertices) * meshData->m_Vertices.size()
									);
									vertexBuffer->SetLayout({
											{ vel::ShaderDataType::Float4, "vColour"},
											{ vel::ShaderDataType::Float4, "vPosition"},
											{ vel::ShaderDataType::Float4, "vNormal"},
											{ vel::ShaderDataType::Float4, "vUVx2"},
											{ vel::ShaderDataType::Float4, "vTangent"},
											{ vel::ShaderDataType::Float4, "vBiNormal"},
											{ vel::ShaderDataType::Float4, "vBoneID"},
											{ vel::ShaderDataType::Float4, "vBoneWeight"}
										});
									meshData->m_VertexArray->AddVertexBuffer(vertexBuffer);
									Ref<IndexBuffer> ib = IndexBuffer::Create(
										&meshData->m_Indices[0],
										meshData->m_Indices.size()
									);
									meshData->m_VertexArray->SetIndexBuffer(ib);
								}
								else continue;
							}

							std::vector<Ref<Texture2D>> textures;
							m_TempShader->Bind();

								bool useDifMap = false;
								bool useSpeMap = false;
								bool useNorMap = false;

								if (material->DiffuseTexture != nullptr && material->DiffuseTexture->IsLoaded())
								{
									useDifMap = true;
								}
								m_TempShader->SetFloat4("RGBA", material->Diffuse);

								if (material->SpecularTexture != nullptr && material->SpecularTexture->IsLoaded())
								{
									useSpeMap = true;
								}
								m_TempShader->SetFloat4("SPEC", material->Specular);

								if (material->NormalTexture != nullptr && material->NormalTexture->IsLoaded())
								{
									useNorMap = true;
								}
								m_TempShader->SetFloat("SHIN", material->Shininess);
								m_TempShader->SetBool("useTextureNormal", useNorMap);
								m_TempShader->SetBool("useTextureSpecular", useSpeMap);
								m_TempShader->SetBool("useTextureDiffuse", useDifMap);

								textures.push_back(material->DiffuseTexture);

								Renderer::Submit(m_TempShader, meshData->m_VertexArray, textures, transform->GetTransform());
						}
					}
				}
			}
		}
		buffer->Unbind();
		m_FBORenderBuffer->Bind();
		RenderCommand::Clear();
		m_ShaderLibrary.Get("FBOTexture")->Bind();
		buffer->BindColorTexture();
		Renderer::Submit(
			m_ShaderLibrary.Get("FBOTexture"),
			m_SquareVertexArray);
		m_FBORenderBuffer->Unbind();
	}
}