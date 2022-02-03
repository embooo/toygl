workspace "ToyGL"
	configurations { "Debug", "Release" }
	architecture "x64"
	startproject "ToyGLEngine"
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- ex: Debug-Windows-x64
	srcdir	  = "ToyEngine/src/"

	include "ToyEngine/thirdparty/Glad"
	include "ToyEngine/thirdparty/GLFW"
	include "ToyEngine/thirdparty/ImGui"

	IncludePaths = {}
	IncludePaths["ToyEngine"]   = "ToyEngine/include/"
	IncludePaths["GLFW"]        = "ToyEngine/thirdparty/Glad/include"
	IncludePaths["Glad"]        = "ToyEngine/thirdparty/GLFW/include"
	IncludePaths["glm"]         = "ToyEngine/thirdparty/glm/"
	IncludePaths["tinygltf"]    = "ToyEngine/thirdparty/tinygltf/"
	IncludePaths["ImGui"]       = "ToyEngine/thirdparty/ImGui/include"
	IncludePaths["Optick"]      = "ToyEngine/thirdparty/Optick"


	
project "ToyEngine"
	location "ToyEngine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	pchheader "PCH.h"
	pchsource (srcdir .. "PCH.cpp")

	files 
	{ 
		IncludePaths.ToyEngine .. "**.h",
		srcdir .. "**.cpp",
		--"ToyEngine/thirdparty/Optick/**cpp",
		".clang-tidy"
	}

	includedirs
	{
		"ToyEngine",
		IncludePaths.ToyEngine,
		IncludePaths.GLFW,
		IncludePaths.Glad,
		IncludePaths.glm,
		IncludePaths.tinygltf,
		IncludePaths.ImGui--,
		--IncludePaths.Optick
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
		defines { "OPTICK_DEBUG", "DEBUG_BUILD" }

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
