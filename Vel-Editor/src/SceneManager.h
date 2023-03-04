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

		void Update(float dt, glm::vec4 eyeLocation);
		void BindSkyBox(int);
		void LoadScene();
		void SaveScene();
		void DrawSkyBox(glm::mat4 viewProjection);
		void BindLightData(Ref<Shader> shader, glm::vec4 eyepos);

		std::string ScenePath;
	private:
		SkyBox* skybox;
		Ref<EntityManager> m_EntityManager;
		ShaderLibrary m_ShaderLibrary;
		Ref<Shader> m_Shader;
		Ref<Shader> m_SkyBoxShader;
	};
}