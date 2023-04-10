#pragma once

#include "Quelos/Scene/Entity.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace Quelos
{
	class EditorGUI
	{
	public:
		static bool CheckBox(const std::string& label, bool& value);
		static bool InputText(const std::string& label, std::string& text, const Vector4& textColor);
		static bool InputFloat(const std::string& label, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f, float columWidth = 100.0f);
		static bool InputVector2(const std::string& label, Vector2& values, float resetValue = 0.0f, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
		static void InputVector3(const std::string& label, Vector3& values, float resetValue = 0.0f, float columWidth = 100.0f);
		static bool InputColor4(const std::string& label, Vector4& values, float resetValue = 1.0f);
		static void TextureSlot(const char* label, Ref<Texture2D>& texture);
		static void TextureSlot(const char* label, Ref<SubTexture2D>& texture);
		static void EntitySlot(const char* label, entt::entity& entityID);
		static bool Button(const std::string& label, bool enabled = true);

		static bool BeginCombo(const std::string& lable, const char* previewType);
		static void EndCombo();

		template <typename GuiFunc>
		static void Combo(const std::string label, const char** comboOptions, uint32_t length, uint32_t curOptionIndex, GuiFunc func)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::Columns(2, nullptr, false);

			ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

			const char* curOption = comboOptions[curOptionIndex];

			if (ImGui::BeginCombo(("##" + label).c_str(), curOption))
			{
				for (int i = 0; i < length; i++)
				{
					bool isSelected = curOption == comboOptions[i];
					if (ImGui::Selectable(comboOptions[i], isSelected))
					{
						curOption = comboOptions[i];
						func(i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
		}
		 
		static bool BeginComponent(const std::string& name, bool* remove = nullptr);
		static void EndComponent(bool remove = false, std::function<void()> onRemove = { });
		
		template<typename ComponentType, typename GuiFunc>
		static void Component(const std::string& lable, Entity entity, GuiFunc func, bool removeable = true)
		{
			bool has = entity.HasComponent<ComponentType>();

			if (!has)
				return;

			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			bool opened = ImGui::TreeNodeEx((void*)&lable, ImGuiTreeNodeFlags_DefaultOpen
											| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth
											| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
											| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding,
											lable.c_str());

			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);

			if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
				ImGui::OpenPopup("ComponentSettings");

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Rest"))
				{
				}

				if (removeable)
				{
					if (ImGui::MenuItem("Remove Component"))
						removeComponent = true;
				}

				ImGui::EndPopup();
			}

			if (opened)
			{
				func(entity, entity.GetComponent<ComponentType>());

				ImGui::TreePop();

				ImGui::Spacing();

				if (removeComponent)
					entity.RemoveComponent<ComponentType>();
			}
		}
	};
}
