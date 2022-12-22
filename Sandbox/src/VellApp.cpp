#include <Vel.h>

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