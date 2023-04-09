#include "SceneHierarchyPanal.h"
#include "../Gui/EditorGUI.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>

namespace Quelos
{
	const extern std::filesystem::path g_AssetsPath;

	SceneHierarchyPanal::SceneHierarchyPanal(const Ref<Scene>& context, InspectorPanel& inspector)
		: m_Inspector(inspector)
	{
		SetContext(context);
	}
	void SceneHierarchyPanal::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_Inspector.SetContext(context);
		m_Inspector.SetSelectedEntity({ });
	}

	void SceneHierarchyPanal::OnImGuiRender()
	{
		static bool show_hierarchy = true;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::MenuItem("Hierarchy", nullptr, &show_hierarchy);
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (show_hierarchy)
		if (ImGui::Begin("Scene Hierarchy", &show_hierarchy))
		{
			m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
				m_Inspector.SetSelectedEntity({ });

			// Right-Click on a blank spcae
			if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
			{
				if (ImGui::MenuItem("Create Entity"))
					m_Context->CreateEntity("New Entity");

				ImGui::EndPopup();
			}

			ImGui::End();
		} else ImGui::End();
	}

	void SceneHierarchyPanal::DrawEntityNode(Entity entity)
	{
		auto& tc = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ((m_Inspector.GetSelectedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)static_cast<uint64_t>((uint32_t)entity), flags, tc.c_str());

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				m_Inspector.SetSelectedEntity(entity);
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			if (ImGui::MenuItem("Duplicate Entity"))
			{
				m_Context->DuplicateEntity(entity);
				m_Inspector.SetSelectedEntity(entity);
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(entt::entity), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

			bool opened = ImGui::TreeNodeEx((void*)651, flags, "Text Child Node");
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_Inspector.GetSelectedEntity() == entity)
				m_Inspector.SetSelectedEntity({ });
		}
	}
}
