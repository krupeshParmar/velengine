#pragma once

#ifdef VEL_PLATFORM_WINDOWS

	#ifdef VEL_BUILD_DLL
		#define VEL_API __declspec(dllexport)
	#else 
		#define VEL_API __declspec(dllimport)
	#endif

#else 
	#error Vel only support Winows!
#endif

#define BIT(x) (1 << x)