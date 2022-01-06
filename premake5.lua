workspace "ToyGL"
	configurations { "Debug", "Release" }
	architecture "x64"
	startproject "ToyGLEngine"
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- ex: Debug-Windows-x64

	include "ToyEngine/thirdparty/Glad"
	include "ToyEngine/thirdparty/GLFW"
	include "ToyEngine/thirdparty/ImGui"


	IncludePaths = {}
	IncludePaths["ToyEngine"] = "ToyEngine/include/"
	IncludePaths["GLFW"]      = "ToyEngine/thirdparty/Glad/include"
	IncludePaths["Glad"]      = "ToyEngine/thirdparty/GLFW/include"
	IncludePaths["glm"]       = "ToyEngine/thirdparty/glm/"
	IncludePaths["tinygltf"]  = "ToyEngine/thirdparty/tinygltf/"
	IncludePaths["ImGui"]     = "ToyEngine/thirdparty/ImGui/include"

	
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
		IncludePaths.ToyEngine,
		IncludePaths.GLFW,
		IncludePaths.Glad,
		IncludePaths.glm,
		IncludePaths.tinygltf,
		IncludePaths.ImGui
	}

	links
	{
		"GLFW", 
		"Glad",
		"ImGui"
	}

	defines
	{
		"GLFW_INCLUDE_NONE"
	}
	
	targetdir	("ToyEngine/build/bin/" .. outputdir )
	objdir		("ToyEngine/build/obj/" .. outputdir )

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		buildoptions {"/Od"}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
