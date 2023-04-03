#pragma once
#include <Vel.h>

class AIProject2 : public vel::Layer
{
public:
	AIProject2();
	~AIProject2();
	void OnUpdate(vel::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(vel::Event& event) override;


private:

	vel::ShaderLibrary m_ShaderLibrary;
	vel::Ref<vel::VertexArray> m_SquareVertexArray;
	vel::Ref<vel::FrameBuffer> fullScreenFrameBuffer;
};