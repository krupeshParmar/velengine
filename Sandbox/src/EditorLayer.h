#pragma once
#include <Vel.h>

class EditorLayer : public vel::Layer
{
public:
	EditorLayer();
	virtual ~EditorLayer() = default;


	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(vel::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(vel::Event& event) override;

private:
	vel::ShaderLibrary m_ShaderLibrary;
	vel::Ref<vel::Texture2D> m_Texture;
	vel::Ref<vel::Texture2D> m_SecondTexture;
	vel::Ref<vel::VertexArray> m_SquareVertexArray;

	vel::EditorCamera m_EditorCamera;


};