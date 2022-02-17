workspace "RayTracing"
  architecture "x64"
	startproject "RayTracing"
  configurations
  {
    "Debug",
    "Release",
    "Dist"
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Solution include directories
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/third-part/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/third-part/Glad/include"
IncludeDir["glm"] = "%{wks.location}/third-part/glm"
IncludeDir["stb"] = "%{wks.location}/third-part/stb"

-- Includes the GLFW and GLAD premake5's files
group "Dependencies"
	include "third-part/GLFW"
	include "third-part/Glad"
group ""

project "RayTracing"
	location "RayTracing"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{wks.location}/third-part/stb/stb_image.h",
		"%{wks.location}/third-part/stb/stb_image.cpp",
		"%{wks.location}/third-part/glm/glm/**.hpp",
		"%{wks.location}/third-part/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLM_ENABLE_EXPERIMENTAL"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}"
	}

	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	disablewarnings  {
		"26451"
	}

	filter "system:windows"
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
