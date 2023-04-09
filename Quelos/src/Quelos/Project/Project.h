#pragma once

#include <filesystem>

namespace Quelos
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		ProjectConfig& GetConfig() { return m_Configuration; }

		static std::filesystem::path GetAssetDirectory()
		{
			QS_CORE_ASSERT(s_ActiveProject, "No active project found");
			return GetProjectDirectory() / s_ActiveProject->m_Configuration.AssetDirectory;
		}
		static const std::filesystem::path& GetProjectDirectory()
		{
			QS_CORE_ASSERT(s_ActiveProject, "No active project found");
			return s_ActiveProject->m_ProjectDirectory;
		}
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			QS_CORE_ASSERT(s_ActiveProject, "No active project found");
			return GetAssetDirectory() / path;
		}
		static Ref<Project> GetActiveProject() { return s_ActiveProject; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

	private:
		ProjectConfig m_Configuration;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<Project> s_ActiveProject;
	};
}
