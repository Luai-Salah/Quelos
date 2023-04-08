#pragma once

#include <filesystem>
#include "Quelos/Renderer/Texture.h"
#include "InspectorPanel.h"

namespace Quelos
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void SetAssetsPath(const std::filesystem::path& path);
		void OnImGuiRender();
	private:
		void DrawNode(std::filesystem::directory_entry directory);
	private:
		InspectorPanel& m_Inspector;
		std::filesystem::path m_AssetsDirectory, m_CurrentDirectory;
		Ref<Texture2D> m_DirectoryIcon, m_FileIcon, m_BackButtonIcon;
	};
}
