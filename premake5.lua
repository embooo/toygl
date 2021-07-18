workspace "ToyGL"
	configurations { "Debug", "Release" }
	architecture "x64"
	startproject "ToyGLEngine"
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- ex: Debug-Windows-x64
	
project "ToyEngine"
	location "projects/%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("build/obj/" .. outputdir .. "/%{prj.name}")