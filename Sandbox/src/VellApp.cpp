#include <Vel.h>
#include <velpch.h>

class VelApp : public vel::Application
{
public:
	VelApp()
	{

	}

	~VelApp()
	{

	}
};

vel::Application* vel::CreateApplication()
{
	return new VelApp();
}