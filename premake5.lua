workspace "ToyGL"
	configurations { "Debug", "Release" }
	architecture "x64"
	startproject "ToyGLEngine"
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- ex: Debug-Windows-x64

	include "projects/ToyEngine/thirdparty/Glad"
	include "projects/ToyEngine/thirdparty/GLFW"

	IncludePaths = {}
	IncludePaths["GLFW"] = "projects/ToyEngine/thirdparty/Glad/include"
	IncludePaths["Glad"] = "projects/ToyEngine/thirdparty/GLFW/include"
	
project "ToyEngine"
	location "projects/ToyEngine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	files 
	{ 
		"projects/ToyEngine/src/**.h",
		"projects/ToyEngine/src/**.cpp",
		".clang-tidy"
	}

	includedirs
	{
		"%{IncludePaths.GLFW}",
		"%{IncludePaths.Glad}"
	}

	links
	{
		"GLFW", 
		"Glad"
	}

	
	
	targetdir	("build/bin/" .. outputdir .. "/%{prj.name}")
	objdir	("build/obj/" .. outputdir .. "/%{prj.name}")

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		buildoptions { "/W4" }
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
