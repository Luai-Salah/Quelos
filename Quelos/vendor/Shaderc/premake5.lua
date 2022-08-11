project "ShaderC"
	kind "StaticLib"
	cppdialect "C++17"
	language "C++"
    staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"**.h",
		"**.hpp",
		"**.cpp",

		"libshaderc/include/**.h"
	}

	includedirs {
		"libshaderc/include"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
