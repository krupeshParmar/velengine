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

		inline Ref<EntityManager> GetEntityManager() { return m_EntityManager; }

		void Update(float dt);
		void LoadScene();
		void SaveScene();

		std::string ScenePath;
	private:
		Ref<EntityManager> m_EntityManager;
		LightManager m_LightManager;
		ShaderLibrary m_ShaderLibrary;
		Ref<Shader> m_Shader;
	};
}