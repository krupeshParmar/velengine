#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include <memory>

namespace vel {
	class VEL_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};
}

// Core Log macros
#define VEL_CORE_TRACE(...) ::vel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VEL_CORE_INFO(...)  ::vel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VEL_CORE_WARN(...)  ::vel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VEL_CORE_ERROR(...) ::vel::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VEL_CORE_FATAL(...) ::vel::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Cliet Log macros
#define VEL_TRACE(...)      ::vel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VEL_INFO(...)       ::vel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VEL_WARN(...)       ::vel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VEL_ERROR(...)      ::vel::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VEL_FATAL(...)      ::vel::Log::GetCoreLogger()->fatal(__VA_ARGS__)
