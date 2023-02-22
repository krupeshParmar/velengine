#pragma once

#include <stdio.h>


#ifdef VEL_PLATFORM_WINDOWS

extern vel::Application* vel::CreateApplication();

int main(int argc, char** argv)
{
	vel::Log::Init();

	VEL_CORE_INFO("Starting Vel App");
	VEL_PROFILE_BEGIN_SESSION("Startup", "Vel-Profile-Startup.json");
	vel::Application* app = vel::CreateApplication();
	VEL_PROFILE_END_SESSION();

	VEL_PROFILE_BEGIN_SESSION("Startup", "Vel-Profile-Runtime.json");
	app->Run();
	VEL_PROFILE_END_SESSION();

	VEL_PROFILE_BEGIN_SESSION("Startup", "Vel-Profile-Shutdown.json");
	delete app;
	VEL_PROFILE_END_SESSION();
}

#endif