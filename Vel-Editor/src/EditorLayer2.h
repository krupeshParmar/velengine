#pragma once

#pragma once
#include <Vel.h>
#include <entt.hpp>

namespace vel
{
	class EditorLayer2 : public Layer
	{
	public:
		EditorLayer2();
		virtual ~EditorLayer2() = default;


		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(vel::Event& event) override;

	private:
		void SceneHierarchy();
		void Inspector();
		void UI_Toolbar();
		void OpenProject();
		void SaveProject();
		void DrawChildren(Entity entity, std::map<GUID, Entity>& shownEntities);

	private:
		Ref<Scene> m_ActiveScene;
		ShaderLibrary m_ShaderLibrary;
		Ref<Texture2D> m_Texture;
		Ref<FrameBuffer> m_RenderBuffer;
		Ref<FrameBuffer> m_ShadowBuffer;
		Ref<FrameBuffer> m_FullScreenFrameBuffer;
		Ref<FrameBuffer> m_PostProcessFrameBuffer;


		Ref<Texture2D> m_SecondTexture;
		Ref<VertexArray> m_SquareVertexArray;

		// Testing
		Ref<VertexArray> m_TestVertexArray;

		glm::vec2 m_ViewPortSize = { 0, 0 };

		EditorCamera m_EditorCamera;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		entt::entity m_SelectedEntity = entt::null;
		bool openProjectCalled = false;
		bool saveProjectCalled = false;
		bool addComponentCalled = false;
		std::chrono::time_point<std::chrono::steady_clock> starttime = std::chrono::steady_clock::now();
		glm::vec3* rotation = new glm::vec3(0.f);
	};
}