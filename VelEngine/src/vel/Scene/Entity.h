#pragma once
#include <glm/glm.hpp>
#include <vel/Scene/Component.h>
#include "Scene.h"
namespace vel
{
	class Entity
	{
	public:
		Entity() {}
		Entity(entt::entity handle, Scene* scene)
			: m_EntityHandle(handle), m_Scene(scene) {}
		~Entity() {}
		bool enabled = true;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			VEL_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			VEL_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			VEL_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename... T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T...>(m_EntityHandle);
		}

		template<typename... T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.all_of<T...>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			VEL_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponentIfExists()
		{
			m_Scene->m_Registry.remove_if_exists<T>(m_EntityHandle);
		}

		operator uint32_t () const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }
		operator bool() const { return (m_EntityHandle != entt::null) && m_Scene && m_Scene->m_Registry.valid(m_EntityHandle); }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		Entity GetParent() const
		{
			return m_Scene->TryGetEntityWithGUID(GetParentGUID());
		}

		void SetParent(Entity parent)
		{
			Entity currentParent = GetParent();
			if (currentParent == parent)
				return;

			// If changing parent, remove child from existing parent
			if (currentParent)
				currentParent.RemoveChild(*this);

			// Setting to null is okay
			SetParentGUID(parent.GetGUID());

			if (parent)
			{
				auto& parentChildren = parent.Children();
				GUID GUID = GetGUID();
				if (std::find(parentChildren.begin(), parentChildren.end(), GUID) == parentChildren.end())
					parentChildren.emplace_back(GetGUID());
			}
		}

		void SetParentGUID(GUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }

		GUID GetParentGUID() const { return GetComponent<RelationshipComponent>().ParentHandle; }

		std::vector<GUID>& Children() { return GetComponent<RelationshipComponent>().Children; }

		const std::vector<GUID>& Children() const { return GetComponent<RelationshipComponent>().Children; }

		bool RemoveChild(Entity child)
		{
			GUID childId = child.GetGUID();
			std::vector<GUID>& children = Children();
			auto it = std::find(children.begin(), children.end(), childId);
			if (it != children.end())
			{
				children.erase(it);
				return true;
			}

			return false;
		}


		bool IsAncesterOf(Entity entity) const
		{
			const auto& children = Children();

			if (children.empty())
				return false;

			for (GUID child : children)
			{
				if (child == entity.GetGUID())
					return true;
			}

			for (GUID child : children)
			{
				if (m_Scene->GetEntityWithGUID(child).IsAncesterOf(entity))
					return true;
			}

			return false;
		}

		bool IsDescendantOf(Entity entity) const { return entity.IsAncesterOf(*this); }

		TransformComponent& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
		const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle).GetTransform(); }

		std::string& Name() { return /*HasComponent<TagComponent>() ? */ GetComponent<TagComponent>().Tag; }
		const std::string& Name() const { return /*HasComponent<TagComponent>() ? */GetComponent<TagComponent>().Tag; }

		GUID GetGUID() const { return GetComponent<IDComponent>().ID; }
		GUID GetSceneGUID() const { return m_Scene->GetGUID(); }
		GUID GetAssetID() const { return GetComponent<IDComponent>().AssetID; }
		GUID& AssetID() { return GetComponent<IDComponent>().AssetID; }

		void SetScene(Scene* scene) { m_Scene = scene; }

		// TODO Remove later
		Scene* GetScene() { return m_Scene; }

	private:
		entt::entity m_EntityHandle{ entt::null };

		inline static std::string NoName = "Unnamed";
		Scene* m_Scene = nullptr;
		friend class Scene;
	};

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		std::vector<GUID>& Children() { return m_Entity.Children(); }

		Scene* GetScene() { return m_Entity.GetScene(); }

	protected:
		virtual void OnCreate()
		{

		}
		virtual void OnDestroy()
		{

		}
		virtual void OnUpdate(Timestep ts)
		{

		}

	private:
		Entity m_Entity;
		friend class Scene;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateFunction)();
		void(*DestroyInstanceFunction)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateFunction = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyInstanceFunction = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
}