#include "QSPCH.h"
#include "ContentBrowserPanel.h"

#include <imgui.h>

namespace Quelos
{
	ContentBrowserPanel::ContentBrowserPanel()
		: m_Inspector(*InspectorPanel::Get())
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FolderIcon.png",
											{ TextureFilterMode::Tilinear, TextureWrapMode::Repeat });
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
		m_BackButtonIcon = Texture2D::Create("Resources/Icons/ContentBrowser/BackButton.png");
	}

	void ContentBrowserPanel::DrawNode(std::filesystem::directory_entry dir)
	{
		std::string filenameString = dir.path().filename().string();

		if (dir.path().extension().string() == ".qsd")
			return;

		if (ImGui::TreeNodeEx(filenameString.c_str(), dir.is_directory() ? ImGuiTreeNodeFlags_OpenOnArrow :
							  (ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanFullWidth)))
		{
			if (dir.is_directory())
			for (auto& directoryEntry : std::filesystem::directory_iterator(dir))
				DrawNode(directoryEntry);

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				auto path = dir.path();
				if (path.extension().string() == ".png")
				{
					auto relativePath = std::filesystem::relative(path, m_AssetsDirectory);
					m_Inspector.SetSelectedTexture(AssetsManager::GetTexture(relativePath.string()));
				}
			}

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = dir.path().c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			ImGui::TreePop();
		}
	}

	void ContentBrowserPanel::SetAssetsPath(const std::filesystem::path& path)
	{
		m_AssetsDirectory = path;
		m_CurrentDirectory = m_AssetsDirectory;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		static bool showContentBrowser = true;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::MenuItem("Content Browser", nullptr, &showContentBrowser);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (showContentBrowser)
		if (!ImGui::Begin("Content Browser", &showContentBrowser))
			ImGui::End();
		else
		{
			float size = ImGui::GetFrameHeight() - 10.0f;

			if (ImGui::BeginTable("table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable, { 0.0f, ImGui::GetContentRegionAvail().y - 4.0f }))
			{
				ImGui::TableSetupColumn("##tree", ImGuiTableColumnFlags_WidthStretch , ImGui::GetWindowWidth() / 3.0f);
				ImGui::TableSetupColumn("##browse", ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 2.0f / 3.0f);
				ImGui::TableNextColumn();

				if (!m_AssetsDirectory.empty())
				{
					if (ImGui::TreeNode("Assets"))
					{
						int dirCount = 0;
						std::vector<std::filesystem::path> nextDirectories;
						std::vector<std::filesystem::path> curDirectories;

						for (auto& path : std::filesystem::directory_iterator(m_AssetsDirectory))
							DrawNode(path);

						ImGui::TreePop();
					}
				}

				ImGui::TableNextColumn();

				if (ImGui::ImageButton((ImTextureID)m_BackButtonIcon->GetRendererID(), { size, size }))
				{
					if (m_CurrentDirectory != std::filesystem::path(m_AssetsDirectory))
						m_CurrentDirectory = m_CurrentDirectory.parent_path();
				}

				ImGui::Separator();
				ImGui::Spacing();

				static int padding = 16.0f;
				static float thumbnailSize = 90.0f;
				float cellSize = thumbnailSize + static_cast<float>(padding);

				float panalWidth = ImGui::GetWindowWidth() * 2.0f / 3.0f;
				int columnCount = static_cast<int>(panalWidth / cellSize);
				if (columnCount < 1)
					columnCount = 1;

				if (ImGui::BeginTable("table2", columnCount, 0, { 0.0f, ImGui::GetContentRegionAvail().y - 4.0f }))
				{
					if (!m_AssetsDirectory.empty())
					{
						for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
						{
							const auto& path = directoryEntry.path();
							if (path.extension() == ".qsd")
								continue;

							ImGui::TableNextColumn();

							auto relativePath = std::filesystem::relative(path, m_AssetsDirectory);
							std::string filenameString = relativePath.filename().string();

							ImGui::PushID(filenameString.c_str());

							Ref<Texture2D> icon;
							if (relativePath.extension() == ".png")
								icon = AssetsManager::GetTexture(relativePath.string());
							else icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;

							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
							ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
							ImGui::PopStyleColor();

							if (ImGui::BeginDragDropSource())
							{
								const wchar_t* itemPath = relativePath.c_str();
								ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
								ImGui::EndDragDropSource();
							}

							if (ImGui::IsItemHovered())
							{
								if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
								{
									if (path.extension() == ".png")
										m_Inspector.SetSelectedTexture(AssetsManager::GetTexture(relativePath.string()));
								}

								if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
								{
									if (directoryEntry.is_directory())
										m_CurrentDirectory /= path.filename();
								}
							}

							ImGui::TextWrapped(filenameString.c_str());

							ImGui::NextColumn();

							ImGui::PopID();
						}
					}

					ImGui::EndTable();
				}

				ImGui::EndTable();
			}

			// TODO: status bar
			ImGui::End();
		}
	}
}
