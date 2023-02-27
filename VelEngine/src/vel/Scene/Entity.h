#pragma once
#include <vel/Scene/Component.h>
namespace vel
{
	class Entity
	{
	public:
		Entity() {}
		~Entity() {}
		std::string name;
		bool enabled = true;

		inline uint32_t GetID() const { return m_ID; }

		inline void AddChild(Entity* child) { m_Children.push_back(child); }

		inline void SetParent(Entity* parent) { m_Parent = parent; }

		// Set entity ID
		inline void SetID(uint32_t id) { m_ID = id; };
	private:
		uint32_t m_ID;
		std::vector<Entity*> m_Children;
		Entity* m_Parent = nullptr;
	};
}