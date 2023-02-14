#pragma once
#include <memory>

#ifdef VEL_PLATFORM_WINDOWS
	#ifdef VEL_DYNAMIC_LINK
		#ifdef VEL_BUILD_DLL
			#define VEL_API __declspec(dllexport)
		#else 
			#define VEL_API __declspec(dllimport)
		#endif
	#else
		#define VEL_API
	#endif
#else 
	#error Vel only support Winows!
#endif

#ifdef VEL_DEBUG
	#define VEL_ENABLE_ASSERTS
#endif

#ifdef VEL_ENABLE_ASSERTS
	#define VEL_ASSERT(x, ... ) { if(!(x)) { VEL_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();} }
	#define VEL_CORE_ASSERT(x, ... ) { if(!(x)) { VEL_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();} }
#else
#define VEL_ASSERT(x, ... )
#define VEL_CORE_ASSERT(x, ... )

#endif
#define BIT(x) (1 << x)

#define VEL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace vel
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}