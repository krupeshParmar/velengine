#pragma once

#include <stdio.h>

#ifdef VEL_PLATFORM_WINDOWS

extern vel::Application* vel::CreateApplication();

int main(int argc, char** argv)
{
	printf("Starting Vel App");
	auto app = vel::CreateApplication();
	app->Run();
	delete app;
}

#endif