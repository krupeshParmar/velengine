//#pragma once
//#include <Vel.h>
//#include "SceneManager.h"
//
//namespace vel
//{
//	class EditorLayer : public Layer
//	{
//	public:
//		EditorLayer();
//		virtual ~EditorLayer() = default;
//
//
//		virtual void OnAttach() override;
//		virtual void OnDetach() override;
//		virtual void OnUpdate(Timestep ts) override;
//		virtual void OnImGuiRender() override;
//		virtual void OnEvent(vel::Event& event) override;
//
//	private:
//		void SceneHierarchy();
//		void Inspector();
//		void UI_Toolbar();
//		void OpenProject();
//		void SaveScene();
//
//	private:
//		Ref<Scene> m_ActiveScene;
//		Scope<SceneManager> m_SceneManager;
//
//		ShaderLibrary m_ShaderLibrary;
//		Ref<Texture2D> m_Texture;
//		Ref<Texture2D> m_NoiseTexture;
//		Ref<FrameBuffer> m_RenderBuffer;
//		Ref<FrameBuffer> m_FullScreenFrameBuffer;
//		Ref<FrameBuffer> m_PostProcessFrameBuffer;
//
//		// Horizontal Blur
//		Ref<FrameBuffer> m_PingFrameBuffer;
//
//		// Vertical Blur
//		Ref<FrameBuffer> m_PongFrameBuffer;
//
//		// TEMP
//		Ref<FrameBuffer> m_BedroomFrameBuffer;
//
//
//		Ref<Texture2D> m_SecondTexture;
//		Ref<VertexArray> m_SquareVertexArray;
//
//		// Testing
//		Ref<VertexArray> m_TestVertexArray;
//		Ref<Model> mTestModel;
//
//		glm::vec2 m_ViewPortSize = { 0, 0 };
//
//		EditorCamera m_EditorCamera;
//		bool m_ViewportFocused = false;
//		bool m_ViewportHovered = false;
//
//		int m_SelectedEntity = -1;
//		bool openProjectCalled = false;
//		bool saveProjectCalled = false;
//		bool addComponentCalled = false;
//		std::chrono::time_point<std::chrono::steady_clock> starttime = std::chrono::steady_clock::now();
//
//		TransformComponent copiedTransform;
//		LightComponent copiedLight;
//		MeshComponent copiedMesh;
//	};
//}