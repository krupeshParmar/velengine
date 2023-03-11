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

		// Set entity ID
		inline void SetID(uint32_t id) { m_ID = id; };

		inline void SetParent(uint32_t parentdID) { m_ParentID = parentdID; }

		inline uint32_t GetParentID() { return m_ParentID; }

	private:
		uint32_t m_ID;
		uint32_t m_ParentID;
	};
}