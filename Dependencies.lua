VULKAN_SDK = os.getenv("VULKAN_SDK")
MonoProject = os.getenv("MonoProject")

IncludeDir = {}
IncludeDir["Box2D"] = "%{wks.location}/Quelos/vendor/box2d/include"
IncludeDir["EnTT"] = "%{wks.location}/Quelos/vendor/Entt/include"
IncludeDir["filewatch"] = "%{wks.location}/Quelos/vendor/filewatch"
IncludeDir["GLFW"] = "%{wks.location}/Quelos/vendor/glfw/include"
IncludeDir["Glad"] = "%{wks.location}/Quelos/vendor/Glad/include"
IncludeDir["glm"] = "%{wks.location}/Quelos/vendor/glm"
IncludeDir["ImGui"] = "%{wks.location}/Quelos/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Quelos/vendor/ImGuizmo"
IncludeDir["Mono"] = "%{wks.location}/Quelos/vendor/mono/include"
IncludeDir["msdfgen"] = "%{wks.location}/Quelos/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/Quelos/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["Shaderc"] = "%{wks.location}/Quelos/vendor/Shaderc/libshaderc/include"
IncludeDir["SpirVCross"] = "%{wks.location}/Quelos/vendor/SpirV-Cross"
IncludeDir["spdlog"] = "%{wks.location}/Quelos/vendor/spdlog/include"
IncludeDir["stb_image"] = "%{wks.location}/Quelos/vendor/stb_image"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Quelos/vendor/yaml-cpp/include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Lib"

LibraryDir["Mono"] = "%{wks.location}/Quelos/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

Library["Mono"] = "%{LibraryDir.Mono}/libmono-static-sgen.lib"

-- Windows --

Library["WinSock"] = "Ws2_32.lib"
Library["WinMeltiMedia"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["WinBCrypt"] = "Bcrypt.lib"
