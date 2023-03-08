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

		void CreateNewScene();
		void Update(Ref<FrameBuffer> buffer,float dt, glm::vec4 eyeLocation);
		void BindSkyBox(int);
		void LoadScene();
		void SaveScene();
		void DrawSkyBox(glm::mat4 viewProjection);
		void BindLightData(Ref<Shader> shader, glm::vec4 eyepos);

		// Temp for project
		void DrawBedroom(Ref<FrameBuffer> buffer,glm::vec4 eyeLocation);
		Ref<FrameBuffer> m_RenderBuffer;
		Ref<FrameBuffer> m_FBORenderBuffer;
		Ref<Entity> ToBeDrawn;

		std::string ScenePath;
		Ref<Shader> m_Shader;
		float Exposure = 0.1f;
		float BloomIntensity = 0.02f;
		float BloomThreshold = 1.f;

		bool UseBloom = true;
	private:
		SkyBox* skybox;
		Ref<EntityManager> m_EntityManager;
		ShaderLibrary m_ShaderLibrary;
		Ref<Shader> m_SkyBoxShader;

		// Temp for project
		Ref<EntityManager> m_TempEntityManager;
		Ref<VertexArray> m_SquareVertexArray;
		Ref<Shader> m_TempShader;
		bool CanLoadTheScene = false;
	};
}