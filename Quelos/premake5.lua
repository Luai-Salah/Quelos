project "Quelos"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "QSPCH.h"
	pchsource "src/QSPCH.cpp"

	files {
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",

		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs {
		"src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.EnTT}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Shaderc}",
		"%{IncludeDir.SpirVCross}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.Mono}"
	}

	links {
		"GLFW",
		"Box2D",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"%{Library.Mono}",
		"opengl32.lib"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		links {
			"%{Library.WinSock}",
			"%{Library.WinMeltiMedia}",
			"%{Library.WinVersion}",
			"%{Library.WinBCrypt}"
		}

		defines {
			"QS_PLATFORM_WINDOWS",
			"QS_ENABLE_ASSERT"
		}

		filter "configurations:Debug"
			defines "QS_DEBUG"
			runtime "Debug"
			symbols "on"

			links {
				"%{Library.ShaderC_Debug}",
				"%{Library.SPIRV_Cross_Debug}",
				"%{Library.SPIRV_Cross_GLSL_Debug}"
			}

		filter "configurations:Release"
			defines "QS_RELEASE"
			runtime "Release"
			optimize "on"

			links {
				"%{Library.ShaderC_Release}",
				"%{Library.SPIRV_Cross_Release}",
				"%{Library.SPIRV_Cross_GLSL_Release}"
			}

		filter "configurations:Dist"
			defines "QS_DIST"
			runtime "Release"
			optimize "on"

			links {
				"%{Library.ShaderC_Release}",
				"%{Library.SPIRV_Cross_Release}",
				"%{Library.SPIRV_Cross_GLSL_Release}"
			}
