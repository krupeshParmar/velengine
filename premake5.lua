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
IncludeDir["PhysxPhysics"] = "%{wks.location}/Libraries/PhysxPhysics/src"
IncludeDir["Assimp"] = "%{wks.location}/VelEngine/vendor/assimp3.2/include"
IncludeDir["entt"] = "%{wks.location}/VelEngine/vendor/entt/include"


Library = {}
Library["Assimp_Debug"] = "%{wks.location}/VelEngine/vendor/assimp3.2/bin/Debug/assimp-vc130-mtd.lib"
Library["Assimp_Release"] = "%{wks.location}/VelEngine/vendor/assimp3.2/bin/Release/assimp-vc130-mt.lib"
--Library["Assimp_Debug"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Debug/assimp-vc143-mtd.lib"
--Library["Assimp_Release"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Release/assimp-vc143-mt.lib"

PhysicsLibs = {}
PhysicsLibs["PHYSX_DEBUG"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug"
PhysicsLibs["PHYSX_RELEASE"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/release"


Binaries = {}
--Binaries["Assimp_Debug"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Debug/assimp-vc143-mtd.dll"
--Binaries["Assimp_Release"] = "%{wks.location}/VelEngine/vendor/assimp/bin/Release/assimp-vc143-mt.dll"
Binaries["Assimp_Debug"] = "%{wks.location}/VelEngine/vendor/assimp3.2/bin/Debug/assimp-vc130-mtd.dll"
Binaries["Assimp_Release"] = "%{wks.location}/VelEngine/vendor/assimp3.2/bin/Release/assimp-vc130-mt.dll"

PhysicsBins = {}
PhysicsBins["PHYSX_DEBUG"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug/*.dll"
PhysicsBins["PHYSX_RELEASE"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/release/*.dll"

group "Dependencies"
include "VelEngine/vendor/GLFW"
include "VelEngine/vendor/Glad"
include "VelEngine/vendor/imgui"
include "VelEngine/vendor/pugixml"
group ""

group "Physics"
include "Libraries/iPhysics"
include "Libraries/VelPhysics"
include "Libraries/PhysxPhysics"
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
		"%{IncludeDir.VelPhysics}",
		"%{IncludeDir.PhysxPhysics}",
		"Libraries/PhysxPhysics/vendor/PhysX/physx/include",
		"%{IncludeDir.entt}",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"iPhysics",
		"VelPhysics",
		"PhysxPhysics",
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

		links
		{
			"%{PhysicsLibs.PHYSX_DEBUG}/LowLevel_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/LowLevelAABB_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/LowLevelDynamics_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysX_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXCharacterKinematic_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXCommon_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXCooking_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXExtensions_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXFoundation_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXPvdSDK_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXTask_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXVehicle_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PhysXVehicle2_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/PVDRuntime_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/SceneQuery_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/SimulationController_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/SnippetRender_static_64.lib",
			"%{PhysicsLibs.PHYSX_DEBUG}/SnippetUtils_static_64.lib",
		}
		

	filter "configurations:Release"
		defines "VEL_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{PhysicsLibs.PHYSX_RELEASE}/LowLevel_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/LowLevelAABB_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/LowLevelDynamics_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysX_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXCharacterKinematic_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXCommon_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXCooking_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXExtensions_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXFoundation_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXPvdSDK_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXTask_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXVehicle_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXVehicle2_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PVDRuntime_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SceneQuery_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SimulationController_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SnippetRender_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SnippetUtils_static_64.lib",
		}

		defines
		{
			"NDEBUG",
		}

	filter "configurations:Dist"
		defines "VEL_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{PhysicsLibs.PHYSX_RELEASE}/LowLevel_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/LowLevelAABB_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/LowLevelDynamics_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysX_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXCharacterKinematic_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXCommon_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXCooking_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXExtensions_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXFoundation_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXPvdSDK_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXTask_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXVehicle_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PhysXVehicle2_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/PVDRuntime_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SceneQuery_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SimulationController_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SnippetRender_static_64.lib",
			"%{PhysicsLibs.PHYSX_RELEASE}/SnippetUtils_static_64.lib",
		}

		defines
		{
			"NDEBUG",
		}

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
		"%{IncludeDir.entt}",
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
		"%{IncludeDir.entt}",
	}

	links
	{
		"VelEngine",
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
			'{COPY} "%{PhysicsBins.PHYSX_DEBUG}" "%{cfg.targetdir}"',
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
			'{COPY} "%{PhysicsBins.PHYSX_RELEASE}" "%{cfg.targetdir}"',
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
			'{COPY} "%{PhysicsBins.PHYSX_RELEASE}" "%{cfg.targetdir}"',
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"'
		}