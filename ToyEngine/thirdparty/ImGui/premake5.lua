project "ImGui"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir    ("obj/" .. outputdir .. "/%{prj.name}")

    IncludePaths = {}
    IncludePaths["GLFW"] = "../GLFW/include"
    files
    {
        "src/imgui.cpp",
        "src/imgui_draw.cpp",
        "src/imgui_demo.cpp",
        "src/imgui_tables.cpp",
        "src/imgui_widgets.cpp",
        "src/imgui_impl_opengl3.cpp",
        "src/imgui_impl_glfw.cpp",
    }

    includedirs
    {
        "include",
        IncludePaths.GLFW
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

