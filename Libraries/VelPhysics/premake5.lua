project "VelPhysics"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

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
		"%{prj.name}/src",
		"../../VelEngine/vendor/glm",
		"../iPhysics/src",
	}

	links
	{
		"iPhysics",
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
