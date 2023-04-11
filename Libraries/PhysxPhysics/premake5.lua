project "PhysxPhysics"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
    staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	--pchheader "pch.h"
	--pchsource "PhysxPhysics/src/pch.cpp"

	PhysicsLibs = {}
	PhysicsLibs["PHYSX_DEBUG"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug"
	PhysicsLibs["PHYSX_RELEASE"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/release"

	
	PhysicsBins = {}
	PhysicsBins["PHYSX_DEBUG"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug"
	PhysicsBins["PHYSX_RELEASE"] = "%{wks.location}/Libraries/PhysxPhysics/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/release"

	files
	{
		"src/**.h",
		"src/**.c",
		"src/**.hpp",
		"src/**.cpp",
		"../../VelEngine/vendor/glm/glm/**.inl",
		"../../VelEngine/vendor/glm/glm/**.hpp",
	}

	includedirs
	{
		"src",
		"vendor/PhysX/physx/include",
		"../../VelEngine/vendor/glm",
		"../iPhysics/src"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
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
			"iPhysics",
		}
		
		postbuildcommands 
		{
			--'{COPY} "%{Binaries.PHYSX_DEBUG}" "%{cfg.targetdir}"',
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

		defines
		{
			"NDEBUG"
		}
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
			"iPhysics",
		}
		
		postbuildcommands 
		{
			--'{COPY} "%{PhysicsBins.PHYSX_RELEASE}" "%{cfg.targetdir}"',
		}

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"

		defines
		{
			"NDEBUG"
		}
		links
		{
			"%{Library.PHYSX_RELEASE}",
			"iPhysics",
		}
		
		postbuildcommands 
		{
			--'{COPY} "%{PhysicsBins.PHYSX_RELEASE}" "%{cfg.targetdir}"',
		}
