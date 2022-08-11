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

		void OnImGuiRender();
	private:
		void DrawNode(std::filesystem::directory_entry directory);
	private:
		InspectorPanel& m_Inspector;
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_DirectoryIcon, m_FileIcon, m_BackButtonIcon;
	};
}
