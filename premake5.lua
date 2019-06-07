workspace "Engine"
	architecture "x64"
	startproject "Client"
	
	configurations
	{
		"Debug",
		"Release"
	}
	
group "Dependencies"
	include "Common/vendor/GLFW"
	include "Common/vendor/enet"
	include "Client/vendor/Glad"
	include "Client/vendor/imgui"

group ""

project "Common"
	location "Common"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir "bin/%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}"
	objdir "bin/int/%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}"

	pchheader "pch.h"
	pchsource "%{prj.name}/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/enet/include",
		"%{prj.name}/intersect"
	}
	
	links
	{
		"GLFW"
	}
	
	defines
	{
		"GLFW_INCLUDE_NONE"
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		
		
		
project "Client"
	location "Client"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir "bin/%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}"
	objdir "bin/int/%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/Glad/include",
		"%{prj.name}/vendor/imgui",
		"Common/src",
		"Common/src/physics",
		"Common/vendor",
		"Common/vendor/GLFW/include",
		"Common/vendor/enet/include"
	}

	links
	{
		"Common",
		"Glad",
		"imgui",
		"ws2_32.lib",
		"winmm.lib",
		"enet"
	}
	
	defines
	{
	
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		
		
		
project "Server"
	location "Server"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir "bin/%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}"
	objdir "bin/int/%{prj.name}/%{cfg.buildcfg}-%{cfg.architecture}"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"Common/src",
		"Common/src/physics",
		"Common/vendor",
		"Common/vendor/GLFW/include",
		"Common/vendor/enet/include"
	}

	links
	{
		"Common",
		"ws2_32.lib",
		"winmm.lib",
		"enet"
	}
	
	defines
	{

	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
