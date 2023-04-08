project "Quelos-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.8"

	targetdir ("../QuelosEditor/Resources/Scripts")
	objdir ("../QuelosEditor/Resources/Scripts/Intermediates")

	files {
		"Source/**.cs",
		"Properties/**.cs"
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
