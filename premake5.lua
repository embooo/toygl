workspace "ToyGL"
    location "build"
	configurations { "Debug", "Release" }
	architecture "x64"
	startproject "ToyGLEngine"
	
	outputdir = "%{cfg.buildcfg}%-%{cfg.system}-{cfg.architecture}-" -- ex: Debug-Windows-x64
	
project "ToyEngine"
	location "build/%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	targetdir ("bin/" .. outputdir .. "%{prj.name}")
	objdir    ("obj/" .. outputdir .. "%{prj.name}")