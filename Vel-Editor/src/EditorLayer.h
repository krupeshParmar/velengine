#pragma once
#include <Vel.h>

namespace vel
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;


		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void UI_Toolbar();
		virtual void OnEvent(vel::Event& event) override;

	private:
		ShaderLibrary m_ShaderLibrary;
		Ref<Texture2D> m_Texture;
		Ref<FrameBuffer> m_RenderBuffer;
		Ref<FrameBuffer> m_FullScreenFrameBuffer;
		Ref<Texture2D> m_SecondTexture;
		Ref<VertexArray> m_SquareVertexArray;

		glm::vec2 m_ViewPortSize = { 0, 0 };

		EditorCamera m_EditorCamera;
	};
}