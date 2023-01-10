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

#ifdef VEL_ENABLE_ASSERTS
	#define VEL_ASSERT(x, ... ) { if(!x) { VEL_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();} }
	#define VEL_CORE_ASSERT(x, ... ) { if(!x) { VEL_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();} }
#else
#define VEL_ASSERT(x, ... )
#define VEL_CORE_ASSERT(x, ... )

#endif
#define BIT(x) (1 << x)