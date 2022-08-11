project "QuelosEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp",
	}

	includedirs {
		"%{wks.location}/Quelos/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.EnTT}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}"
	}

	links {
		"Quelos"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"QS_PLATFORM_WINDOWS"
		}

		filter "configurations:Debug"
			defines "QS_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "QS_RELEASE"
			runtime "Release"
			optimize "on"

		filter "configurations:Dist"
			defines "QS_DIST"
			runtime "Release"
			optimize "on"