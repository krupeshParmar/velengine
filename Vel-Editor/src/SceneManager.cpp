#include "SceneManager.h"

namespace vel
{
	SceneManager::SceneManager()
	{
		m_EntityManager.CreateEntity("First GameObject");
		m_EntityManager.CreateEntity("Second GameObject");
		m_EntityManager.CreateEntity("Third GameObject");
		m_EntityManager.CreateEntity("Fourth GameObject");
		m_EntityManager.CreateEntity("Fifth GameObject");
	}
	SceneManager::~SceneManager()
	{
	}
}