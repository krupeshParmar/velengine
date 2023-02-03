#include <Vel.h>
#include <velpch.h>
#include <imgui/imgui.h>

class ExampleLayer : public vel::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{

	}

	void OnUpdate() override
	{
		if(vel::Input::IsKeyPressed(KeyCode::Tab))
			VEL_INFO("Tab key is pressed");
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World!");
		ImGui::End();
	}

	void OnEvent(vel::Event& event) override
	{
		VEL_TRACE("{0}", event);
	}
};

class VelApp : public vel::Application
{
public:
	VelApp()
	{
		PushLayer(new ExampleLayer());
	}

	~VelApp()
	{

	}
};

vel::Application* vel::CreateApplication()
{
	return new VelApp();
}