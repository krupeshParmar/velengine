//#pragma once
//#include <Vel.h>
//#include <string>
//#include <vector>
//#include <chrono>
//
//namespace vel
//{
//	class SceneManager
//	{
//	public:
//		SceneManager();
//		~SceneManager();
//
//		inline Ref<EntityManager> GetEntityManager() { return m_EntityManager; }
//
//		void CreateNewScene();
//		void Update(Ref<FrameBuffer> buffer,float dt, glm::vec4 eyeLocation);
//		void BindSkyBox(int);
//		void LoadScene();
//		void SaveScene();
//		void DrawSkyBox(glm::mat4 viewProjection);
//		void BindLightData(Ref<Shader> shader, glm::vec4 eyepos);
//
//		// Temp for project
//		void DrawInventory(Ref<FrameBuffer> buffer,glm::vec4 eyeLocation, float dt);
//		void DrawSecurityCameraView(float dt);
//		Ref<FrameBuffer> m_RenderBuffer;
//		Ref<FrameBuffer> m_FBORenderBuffer;
//		Ref<FrameBuffer> m_SecRenderBuffer;
//		Ref<FrameBuffer> m_FBOSecRenderBuffer;
//		Ref<Entity> ToBeDrawn;
//
//		std::string ScenePath;
//		Ref<Shader> m_Shader;
//		float Exposure = 0.1f;
//		float BloomIntensity = 0.02f;
//		float BloomThreshold = 1.f;
//
//		bool UseBloom = true;
//	private:
//		SkyBox* skybox;
//		Ref<EntityManager> m_EntityManager;
//		ShaderLibrary m_ShaderLibrary;
//		Ref<Shader> m_SkyBoxShader;
//		glm::vec3 skyboxRotation;
//		float step = 0;
//
//		// Temp for project
//		Ref<EntityManager> m_TempEntityManager;
//		Ref<VertexArray> m_SquareVertexArray;
//		Ref<Shader> m_TempShader;
//		bool CanLoadTheScene = false;
//		std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
//	};
//}