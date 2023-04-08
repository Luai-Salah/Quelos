#include "QSPCH.h"
#include "ProjectSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Quelos
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;

		out << YAML::BeginMap; // Root
		{
			out << YAML::Key << "Project" << YAML::Value;

			out << YAML::BeginMap; // Project Configurations
			{
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
			}
			out << YAML::EndMap; // Project Configurations
		}
		out << YAML::EndMap; // Root

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;

		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (const YAML::ParserException& ex)
		{
			QS_CORE_ERROR("Failed to load project file from path: '{0}'. (YAML Exeption: {1})", filepath.string(), ex.what());
			return false;
		}

		auto project = data["Project"];
		if (!project)
			return false;

		config.Name = project["Name"].as<std::string>();
		config.StartScene = project["StartScene"].as<std::string>();
		config.AssetDirectory = project["AssetDirectory"].as<std::string>();
		config.ScriptModulePath = project["ScriptModulePath"].as<std::string>();

		return true;
	}
}
