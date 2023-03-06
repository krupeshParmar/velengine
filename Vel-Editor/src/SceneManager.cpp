#include "SceneManager.h"
CRITICAL_SECTION SceneManagerEntitesList_Lock;

namespace vel
{
	SceneManager::SceneManager()
	{
		InitializeCriticalSection(&SceneManagerEntitesList_Lock);
		std::string maria = "assets/models/maria/Maria.fbx";
		std::string rfa = "assets/models/rfa/rfa_separate_cloth.fbx";
		std::string nightshade = "assets/models/nightshade/Nightshade J Friedrich@Idle.fbx";
		std::string medea = "assets/models/medea/medea_m_arrebola.fbx";
		std::string props = "assets/models/camp/props.fbx";
		std::string bench = "assets/models/bench/bench.fbx";

		m_EntityManager = CreateRef<EntityManager>();
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
		m_Shader->Bind();
		m_Shader->SetInt("u_TextureDiffuse", 0);
		m_Shader->SetInt("u_TextureNormal", 1);
		m_Shader->SetInt("u_TextureSpecular", 2);
		m_Shader->SetInt("skyBox", 3);


		m_SkyBoxShader = m_ShaderLibrary.Load("assets/shaders/SkyBox.glsl");
		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetInt("skybox", 0);

		ScenePath = "MariaTest";
		LoadScene();
	}
	SceneManager::~SceneManager()
	{
		DeleteCriticalSection(&SceneManagerEntitesList_Lock);
	}
	void SceneManager::Update(float dt, glm::vec4 eyeLocation)
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
				if (!lightComp->Enabled)
					continue;
				lightComp->Position = glm::vec4(transform->Translation, 1.0);
				lightComp->Direction = glm::vec4(transform->GetRotation(), 1.0);
			}
			
			if (m_EntityManager->HasComponent<MeshComponent>(entity->GetID()))
			{
				MeshComponent* meshComp = m_EntityManager->GetComponentByType<MeshComponent>(entity->GetID());
				if (!meshComp->Enabled)
					continue;
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
					m_Shader->Bind();
					BindSkyBox(3);
					m_Shader->SetFloat4("eyeLocation", eyeLocation);
					meshComp->ModelIns->DrawMesh(m_Shader, meshComp->MaterialIns, transform->GetTransform());
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
		LoadSceneFile(path, m_EntityManager);
	}
	void SceneManager::SaveScene()
	{
		const std::string path = "assets/scenes/" + ScenePath + ".xml";
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
}