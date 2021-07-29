workspace "ToyGL"
	configurations { "Debug", "Release" }
	architecture "x64"
	startproject "ToyGLEngine"
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- ex: Debug-Windows-x64

	include "ToyEngine/thirdparty/Glad"
	include "ToyEngine/thirdparty/GLFW"

	IncludePaths = {}
	IncludePaths["ToyEngine"] = "ToyEngine/include/"
	IncludePaths["GLFW"] = "ToyEngine/thirdparty/Glad/include"
	IncludePaths["Glad"] = "ToyEngine/thirdparty/GLFW/include"
	IncludePaths["glm"]  = "ToyEngine/thirdparty/glm/"
	
project "ToyEngine"
	location "ToyEngine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	files 
	{ 
		"ToyEngine/include/**.h",
		"ToyEngine/src/**.cpp",
		".clang-tidy"
	}

	includedirs
	{
		"%{IncludePaths.ToyEngine}",
		"%{IncludePaths.GLFW}",
		"%{IncludePaths.Glad}",
		"%{IncludePaths.glm}"
	}

	links
	{
		"GLFW", 
		"Glad"
	}
	
	targetdir	("ToyEngine/build/bin/" .. outputdir )
	objdir		("ToyEngine/build/obj/" .. outputdir )

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		buildoptions { "/W4" }
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
