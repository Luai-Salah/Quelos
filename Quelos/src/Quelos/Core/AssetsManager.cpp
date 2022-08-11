#include "QSPCH.h"
#include "AssetsManager.h"

#include "Quelos/Renderer/Texture.h"

#include <yaml-cpp/yaml.h>

namespace Quelos
{
	extern const std::filesystem::path g_AssetsPath = "Assets";

	struct AssetsManagerData
	{
		std::unordered_map<std::string, Ref<Texture2D>> Textures;
		std::unordered_map<std::string, std::filesystem::path> Scenes;
		std::unordered_map<std::string, std::filesystem::directory_entry> Directories;
	};

	static AssetsManagerData s_Data;

	void AssetsManager::Init()
	{
		UpdateAssetsFolder();
	}

	void AssetsManager::SerializeAsset(const Ref<Texture2D>& texture)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "TextureName" << YAML::Value << texture->GetName();

		out << YAML::Key << "Specifications" << YAML::Value;
		out << YAML::BeginMap; // Specifications

		TextureSpecification spec = texture->GetSpecification();
		out << YAML::Key << "FilterMode" << YAML::Value << (int)spec.FilterMode;
		out << YAML::Key << "WrapMode" << YAML::Value << (int)spec.WrapMode;

		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(texture->GetPath().string() + ".qsd");
		fout << out.c_str();
	}

	static bool DeserializeAsset(const Ref<Texture2D>& texture, const std::filesystem::path& path)
	{
		std::ifstream stream(path);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		std::string s = strStream.str();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["TextureName"])
			return false;

		YAML::Node yd = data["Specifications"];

		TextureSpecification spec((TextureFilterMode)yd["FilterMode"].as<int>(), (TextureWrapMode)yd["WrapMode"].as<int>());
		texture->SetSpecification(spec);
		texture->Apply();

		return true;
	}

	void AssetsManager::UpdateAssetsFolder()
	{
		int dirCount = 0;
		std::vector<std::filesystem::path> nextDirectories;
		std::vector<std::filesystem::path> curDirectories;

		s_Data.Directories["Assets"] = std::filesystem::directory_entry(g_AssetsPath);

		for (auto& directoryEntry : std::filesystem::directory_iterator(g_AssetsPath))
		{
			const auto& path = directoryEntry.path();

			auto relativePath = std::filesystem::relative(path, g_AssetsPath);

			if (directoryEntry.is_directory())
			{
				s_Data.Directories[path.string()] = directoryEntry;
				nextDirectories.push_back(directoryEntry.path());
				dirCount++;
			}
			else
			{
				if (relativePath.extension() == ".png")
				{
					s_Data.Textures[path.string()] = Texture2D::Create(path);
					if (!std::filesystem::exists(path.string() + ".qsd"))
						SerializeAsset(s_Data.Textures[path.string()]);
					else DeserializeAsset(s_Data.Textures[path.string()], path.string() + ".qsd");
				}
				if (relativePath.extension() == ".quelos")
					s_Data.Scenes[path.string()] = path;
				if (relativePath.extension() == ".qsd")
				{

				}
			}
		}

		while (dirCount != 0)
		{
			dirCount = 0;

			curDirectories = nextDirectories;
			nextDirectories.clear();

			for (auto& path : curDirectories)
			{
				for (auto& directoryEntry : std::filesystem::directory_iterator(path))
				{
					const auto& path = directoryEntry.path();

					auto relativePath = std::filesystem::relative(path, g_AssetsPath);
					std::string filenameString = relativePath.filename().string();

					if (directoryEntry.is_directory())
					{
						s_Data.Directories[path.string()] = directoryEntry;
						nextDirectories.push_back(directoryEntry.path());
						dirCount++;
					}
					else
					{
						if (relativePath.extension() == ".png")
						{
							s_Data.Textures[path.string()] = Texture2D::Create(path);
							if (!std::filesystem::exists(path.string() + ".qsd"))
								SerializeAsset(s_Data.Textures[path.string()]);
							else DeserializeAsset(s_Data.Textures[path.string()], path.string() + ".qsd");
						}
						if (relativePath.extension() == ".quelos")
							s_Data.Scenes[path.string()] = path;
					}
				}
			}
		}
	}

	const Ref<Texture2D>& AssetsManager::GetTexture(std::string name) { return s_Data.Textures[name]; }

	std::filesystem::path AssetsManager::GetScene(std::string name) { return s_Data.Scenes[name]; }
	std::filesystem::directory_entry AssetsManager::GetDirectories(std::string name) { return s_Data.Directories[name]; }
}
