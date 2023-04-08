#pragma once

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

		std::filesystem::path GetAssetDirectory();
		static Ref<Project> GetActiveProject() { return s_ActiveProject; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

	private:
		ProjectConfig m_Configuration;
		std::filesystem::path m_ActiveProjectDirectory;

		inline static Ref<Project> s_ActiveProject;
	};
}
