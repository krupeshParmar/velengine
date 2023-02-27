workspace "VelEngine"
	architecture "x64"
	startproject "Vel-Editor"

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
IncludeDir["stb_image"] = "VelEngine/vendor/stb_image"
IncludeDir["PugiXML"] = "VelEngine/vendor/pugixml/src"
IncludeDir["iPhysics"] = "Libraries/iPhysics/src"
IncludeDir["VelPhysics"] = "Libraries/VelPhysics/src"
IncludeDir["Assimp"] = "%{wks.location}/VelEngine/vendor/assimp/include"


Library = {}
Library["Assimp_Debug"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Debug/assimp-vc143-mtd.lib"
Library["Assimp_Release"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Release/assimp-vc143-mt.lib"


Binaries = {}
Binaries["Assimp_Debug"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Debug/assimp-vc143-mtd.dll"
Binaries["Assimp_Release"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Release/assimp-vc143-mt.dll"

group "Dependencies"
include "VelEngine/vendor/GLFW"
include "VelEngine/vendor/Glad"
include "VelEngine/vendor/imgui"
include "VelEngine/vendor/pugixml"
group ""

group "Physics"
include "Libraries/iPhysics"
include "Libraries/VelPhysics"
group ""

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
		"%{prj.name}/vendor/stb_image/*.h",
		"%{prj.name}/vendor/stb_image/*.cpp",
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

		"%{IncludeDir.Assimp}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.PugiXML}",
		"%{IncludeDir.iPhysics}",
		"%{IncludeDir.VelPhysics}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"iPhysics",
		"VelPhysics",
		"PugiXML",
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
		"VelEngine/vendor/glm",
		"VelEngine/vendor/pugixml/src",
		"Libraries/iPhysics/src",
		"Libraries/VelPhysics/src",
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

		
project "Vel-Editor"
	location "Vel-Editor"
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
		"VelEngine/vendor/glm",
		"VelEngine/vendor/pugixml/src",
		"Libraries/iPhysics/src",
		"Libraries/VelPhysics/src",
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

		links
		{
			"%{Library.Assimp_Debug}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Debug}" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		defines "VEL_RELEASE"
		runtime "Release"
		optimize "on"
		
		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "VEL_DIST"
		runtime "Release"
		optimize "on"
		
		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"'
		}
