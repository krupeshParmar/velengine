#include "SceneManager.h"

namespace vel
{
	SceneManager::SceneManager()
	{
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


		m_Shader = m_ShaderLibrary.Load("assets/shaders/simpleshader1.glsl");
		m_Shader->Bind();
		m_Shader->SetInt("u_TextureDiffuse", 0);
		m_Shader->SetInt("u_TextureNormal", 1);
		m_Shader->SetInt("u_TextureSpecular", 2);
		ScenePath = "MariaTest";
		LoadScene();
	}
	SceneManager::~SceneManager()
	{
	}
	void SceneManager::Update(float dt)
	{
		m_Shader->Bind();
		m_LightManager.CopyLightInformationToShader(m_Shader);
		for (Entity* entity : *m_EntityManager->GetAllEntities())
		{
			if (!entity->enabled)
				continue;
			TransformComponent* transform = m_EntityManager->GetComponentByType<TransformComponent>(entity->GetID());			
			if (m_EntityManager->HasComponent<LightComponent>(entity->GetID()))
			{
				LightComponent* lightComp = m_EntityManager->GetComponentByType<LightComponent>(entity->GetID());
				lightComp->Position = glm::vec4(transform->Translation, 1.0);
				lightComp->Direction = glm::vec4(transform->GetRotation(), 1.0);
			}
			
			if (m_EntityManager->HasComponent<MeshComponent>(entity->GetID()))
			{
				MeshComponent* meshComp = m_EntityManager->GetComponentByType<MeshComponent>(entity->GetID());
				meshComp->ModelIns->DrawMesh(m_Shader,meshComp->MaterialIns, transform->GetTransform());
			}
		}
	}
	void SceneManager::LoadScene()
	{
		const std::string path = "assets/scenes/" + ScenePath + ".xml";
		if (!LoadSceneFile(path, m_EntityManager))
		{
			int breakme = 0;
		}
	}
	void SceneManager::SaveScene()
	{
		const std::string path = "assets/scenes/" + ScenePath + ".xml";
		if (!SaveSceneFile(path, m_EntityManager))
		{
			int breakme = 0;
		}
	}
}