#pragma once

#include <filesystem>
#include "Quelos/Renderer/Texture.h"

namespace Quelos
{
	class Texture2D;

	class AssetsManager
	{
	public:
		static void Init(const std::filesystem::path& path);

		static void UpdateAssetsFolder();

		static void SerializeAsset(const Ref<Texture2D>& texture);

		static const Ref<Texture2D>& GetTexture(std::string name);
		static std::filesystem::path GetScene(std::string name);
		static std::filesystem::directory_entry GetDirectories(std::string name);

		static std::filesystem::path GetRelativePath(const std::filesystem::path& path);
	};
}
