#pragma once

#include <stdio.h>

#ifdef VEL_PLATFORM_WINDOWS

extern vel::Application* vel::CreateApplication();

int main(int argc, char** argv)
{
	vel::Log::Init();
	VEL_CORE_WARN("Initialized Log!");
	int a = 1;
	VEL_INFO("Hello! Var={0}", a);
	VEL_ERROR("Hello! Var={0}", a);
	VEL_CRITICAL("Hello! Var={0}", a);
	VEL_TRACE("Hello! Var={0}", a);
	printf("Starting Vel App");
	vel::Application* app = vel::CreateApplication();
	app->Run();
	delete app;
}

#endif