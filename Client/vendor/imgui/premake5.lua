project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir "bin/%{cfg.buildcfg}-%{cfg.architecture}"
	objdir "bin/int/%{cfg.buildcfg}-%{cfg.architecture}"

    files
    {
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp"
    }
    
    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Release"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
