#include <Vel.h>
#include <velpch.h>

class ExampleLayer : public vel::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{

	}

	void OnUpdate() override
	{
		//VEL_INFO("ExampleLayer::Update");
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