project "enet"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir "bin/%{cfg.buildcfg}-%{cfg.architecture}"
	objdir "bin/int/%{cfg.buildcfg}-%{cfg.architecture}"
	
	files { "*.c" }
	
	includedirs { "include/" }

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines({ "NDEBUG" })
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines({ "DEBUG" })
		targetsuffix "d"
