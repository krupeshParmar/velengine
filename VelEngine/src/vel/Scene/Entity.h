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

		inline uint32_t GetID() const { return m_ID; }

		// Set entity ID
		inline void SetID(uint32_t id) { m_ID = id; };
	private:
		uint32_t m_ID;
	};
}