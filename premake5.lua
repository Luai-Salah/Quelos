include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Quelos"
	architecture "x64"
	startproject "QuelosEditor"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
	include "Quelos/vendor/Box2D"
	include "Quelos/vendor/GLFW"
	include "Quelos/vendor/Glad"
	include "Quelos/vendor/ImGui"
	include "Quelos/vendor/msdf-atlas-gen"
	include "Quelos/vendor/yaml-cpp"
group ""

group "Core"
	include "Quelos"
	include "Quelos-ScriptCore"
group ""

group "Tools"
	include "QuelosEditor"
group ""