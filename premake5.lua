workspace "VelEngine"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "VelEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "VelEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "VelEngine/vendor/imgui"
IncludeDir["GLM"] = "VelEngine/vendor/glm"

include "VelEngine/vendor/GLFW"
include "VelEngine/vendor/Glad"
include "VelEngine/vendor/imgui"

project "VelEngine"
	location "VelEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "velpch.h"
	pchsource "VelEngine/src/velpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/glm/glm/**.hpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLM}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VEL_PLATFORM_WINDOWS",
			"VEL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "VEL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VEL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "VEL_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	staticruntime "on"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"VelEngine/vendor/spdlog/include",
		"VelEngine/vendor/",
		"VelEngine/src",
		"VelEngine/vendor/glm"
	}

	links
	{
		"VelEngine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VEL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VEL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VEL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "VEL_DIST"
		runtime "Release"
		optimize "on"
