#pragma once
#include <Vel.h>
#include <string>
#include <vector>

namespace vel
{
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		inline EntityManager const GetEntityManager() { return m_EntityManager; }

	private:
		std::string m_ScenePath;
		EntityManager m_EntityManager;
	};
}