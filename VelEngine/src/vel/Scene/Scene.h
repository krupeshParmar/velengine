#pragma once
#include "vel/Events/Event.h"
#include "entt.hpp"
#include <unordered_map>
#include "vel/Core/GUID.h"
#include "vel/Core/Timestep.h"
#include <glm/glm.hpp>
#include "Component.h"
#include <iPhysicsFactory.h>
#include <iPhysicsWorld.h>

namespace vel
{
	class Entity;
	using EntityMap = std::unordered_map<GUID, Entity>;
	class Animation;
	class Animator;

	class Scene
	{
	public:
		Scene();
		~Scene();
		void ReloadShader();
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, glm::vec4 eyeLocation);

		void OnEvent(Event& e);

		void SetViewportSize(uint32_t width, uint32_t height);


		Entity CreateEntity(const std::string& name = "", EntityType type = EntityType::NotAsset);
		Entity CreateChildEntity(Entity parent, const std::string& name = "", EntityType type = EntityType::NotAsset);
		Entity CreateEntityWithID(GUID guid, const std::string& name = "", bool shouldSort = true, EntityType type = EntityType::NotAsset);
		Entity CreateChildEntityWithID(GUID guid, Entity parent, const std::string& name = "", bool shouldSort = true, EntityType type = EntityType::NotAsset);
		Entity CreateAssetEntityWithID(GUID guid, Entity parent, const std::string& name = "", bool shouldSort = true, EntityType type = EntityType::NotAsset);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		// returns entity with id as specified. entity is expected to exist (runtime error if it doesn't)
		Entity GetEntityWithGUID(GUID id) const;

		// returns entity with id as specified. entity is expected to exist (runtime error if it doesn't)
		Entity GetAssetEntityWithGUID(GUID id) const;

		// return entity with id as specified, or empty entity if cannot be found - caller must check
		Entity TryGetEntityWithGUID(GUID id) const;

		// return entity with tag as specified, or empty entity if cannot be found - caller must check
		Entity TryGetEntityWithTag(const std::string& tag);
		
		// return descendant entity with tag as specified, or empty entity if cannot be found - caller must check
		// descendant could be immediate child, or deeper in the hierachy
		Entity TryGetDescendantEntityWithTag(Entity entity, const std::string& tag);

		void ParentEntity(Entity entity, Entity parent);
		void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

		void ConvertToLocalSpace(Entity entity);
		void ConvertToWorldSpace(Entity entity);
		glm::mat4 GetWorldSpaceTransformMatrix(Entity entity);
		TransformComponent GetWorldSpaceTransform(Entity entity);

		GUID GetGUID() const { return m_SceneID; }

		EntityMap& GetEntityMap() { return m_EntityIDMap; }


		// TEMP
		void BindSkyBox(int);
		void LoadScene();
		void SaveScene();
		void DrawSkyBox(glm::mat4 viewProjection);
		void BindLightData(Ref<Shader> shader, glm::vec4 eyepos);
		void LoadAnimation(Animation* animation);
		physics::iPhysicsWorld* GetPhysicsWorld() { return m_PhysicsWorld; }

		std::string ScenePath;
		Ref<Shader> m_Shader;
		bool DepthOfField = false;
		float FocalDistance = 1.f;

		float Exposure = 0.1f;
		float BloomIntensity = 0.02f;
		float BloomThreshold = 0.5f;

		bool UseBloom = true;

		bool useNightVision = false;
		bool useRippleEffect = false;
		bool useGrayscale = false;

	private:
		void SortEntities();
		void AddTransparentEntiy(std::vector<entt::entity>* list, entt::entity);
		bool IsEnabled(Entity entity);

	private:
		GUID m_SceneID;
		EntityMap m_EntityIDMap;
		entt::registry m_Registry;

		SkyBox* skybox;
		ShaderLibrary m_ShaderLibrary;
		Ref<Shader> m_SkyBoxShader;
		glm::vec3 skyboxRotation;
		float step = 0;

		Animator* mainAnimator;
		physics::iPhysicsFactory* m_PhysicsFactory;
		physics::iPhysicsWorld* m_PhysicsWorld;

		friend class Entity;
	};
}