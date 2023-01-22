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
	kind "SharedLib"
	language "C++"
	staticruntime "off"

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
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"VEL_PLATFORM_WINDOWS",
			"VEL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" ..outputdir.. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "VEL_DEBUG"
		runtime "Debug"
		symbols "ON"

	filter "configurations:Release"
		defines "VEL_RELEASE"
		runtime "Release"
		optimize "ON"

	filter "configurations:Dist"
		defines "VEL_DIST"
		runtime "Release"
		optimize "ON"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	staticruntime "off"

	language "C++"

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
		"VelEngine/src",
		"VelEngine/vendor/glm"
	}

	links
	{
		"VelEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"VEL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VEL_DEBUG"
		runtime "Debug"
		symbols "ON"

	filter "configurations:Release"
		defines "VEL_RELEASE"
		runtime "Release"
		optimize "ON"

	filter "configurations:Dist"
		defines "VEL_DIST"
		runtime "Release"
		optimize "ON"
