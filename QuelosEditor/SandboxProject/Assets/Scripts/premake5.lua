QuelosRootDir = "../../.."

workspace "Sandbox"
	architecture "x64"
	startproject "Sandbox"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.8"

	targetdir ("Binaries")
	objdir ("Intermediates")

	files {
		"Source/**.cs",
		"Properties/**.cs"
	}

	links {
		"Quelos-ScriptCore"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "on"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

group "Quelos"
	include(QuelosRootDir .. "/Quelos-ScriptCore")
group ""
