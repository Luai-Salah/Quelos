#include "QSPCH.h"
#include "EditorGUI.h"

#include "Quelos/Core/AssetsManager.h"
#include "Quelos/Scene/Entity.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>

namespace Quelos
{
	const extern std::filesystem::path g_AssetsPath;

	bool EditorGUI::CheckBox(const std::string& label, bool& value)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Spacing();

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - ImGui::GetFrameHeight());
		bool used = ImGui::Checkbox("##c", &value);

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::Spacing();

		ImGui::PopID();

		return used;
	}

	bool EditorGUI::InputText(const std::string& label, std::string& text, const Vector4& textColor = Vector4{ 0 })
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Spacing();

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		if (textColor != Vector4{0})
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(textColor.x, textColor.y, textColor.z, textColor.w));

		static char buffer[64];
		strcpy_s(buffer, sizeof(buffer), text.c_str());

		bool used = ImGui::InputText(("##" + label).c_str(), buffer, sizeof(buffer));
		if (used)
			text = buffer;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		if (textColor != Vector4 { 0 })
			ImGui::PopStyleColor();
		ImGui::Columns(1);

		ImGui::Spacing();

		ImGui::PopID();

		return used;
	}

	bool EditorGUI::InputFloat(const std::string& label, float& value, float speed, float min, float max, float columWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		bool used = ImGui::DragFloat("##f", &value, speed, min, max);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		ImGui::Spacing();

		return used;
	}

	bool EditorGUI::InputVector2(const std::string& label, Vector2& values, float resetValue, float speed, float min, float max)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		bool used = false;

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth((ImGui::GetContentRegionAvail().x - buttonSize.x) / 2.0f);
		used = ImGui::DragFloat("##x", &values.x, speed, min, max, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (!used)
			used = ImGui::DragFloat("##y", &values.y, speed, 0.0f, 0.0f, "%.2f");
		else ImGui::DragFloat("##y", &values.y, speed, min, max, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		ImGui::Spacing();

		return used;
	}

	void EditorGUI::InputVector3(const std::string& label, Vector3& values, float resetValue, float columWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth((ImGui::GetContentRegionAvail().x - buttonSize.x * 2.0f) / 3.0f);
		ImGui::DragFloat("##x", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth((ImGui::GetContentRegionAvail().x - buttonSize.x) / 2.0f);
		ImGui::DragFloat("##y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::DragFloat("##z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		ImGui::Spacing();
	}

	bool EditorGUI::InputColor4(const std::string& label, Vector4& values, float resetValue)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		bool used = ImGui::ColorEdit4("##ce", glm::value_ptr(values));
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		ImGui::Spacing();

		return used;
	}

	bool EditorGUI::BeginCombo(const std::string& label, const char* previewValue)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

		return ImGui::BeginCombo((std::string("##") + label).c_str(), previewValue);
	}

	void EditorGUI::EndCombo()
	{
		ImGui::EndCombo();
		ImGui::PopItemWidth();
	}

	bool EditorGUI::BeginComponent(const std::string& name, bool* remove)
	{
		ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		bool opened = ImGui::TreeNodeEx((void*)&name, ImGuiTreeNodeFlags_DefaultOpen
										| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth
										| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
										| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding,
										name.c_str());

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

			if (remove != nullptr)
			{
				if (ImGui::MenuItem("Remove Component"))
					*remove = true;
			}

			ImGui::EndPopup();
		}

		return opened;
	}

	void EditorGUI::EndComponent(bool remove, std::function<void()> fn)
	{
		ImGui::TreePop();

		ImGui::Spacing();

		if (remove)
			fn();
	}

	void EditorGUI::TextureSlot(const char* label, Ref<SubTexture2D>& subTexture)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float squareSize = ImGui::GetFrameHeight();
		ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - squareSize);
		uint32_t rendererID = subTexture ? subTexture->GetTexture()->GetRendererID() : 1;
		if (ImGui::ImageButton((ImTextureID)rendererID, { squareSize, squareSize }, { 0, 1 }, { 1, 0 }, 0))
			subTexture = nullptr;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* wpath = static_cast<const wchar_t*>(payload->Data);
				std::filesystem::path path(wpath);
				if (path.extension() == ".png" || ".jpg")
					subTexture = CreateRef<SubTexture2D>(AssetsManager::GetTexture(path.string()));

				ImGui::EndDragDropTarget();
			}
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::Spacing();
	}

	void EditorGUI::EntitySlot(const char* label, entt::entity& entityID)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float squareSize = ImGui::GetFrameHeight();
		ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - squareSize);
		if (ImGui::Button("##entity", {squareSize, squareSize}))
			entityID = entt::null;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY"))
			{
				entityID = *(entt::entity*)payload->Data;
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::Spacing();
	}

	void EditorGUI::TextureSlot(const char* label, Ref<Texture2D>& texture)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float squareSize = ImGui::GetFrameHeight();
		ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - squareSize);
		uint32_t rendererID = texture ? texture->GetRendererID() : 1;
		if (ImGui::ImageButton((ImTextureID)rendererID, { squareSize, squareSize }, { 0, 1 }, { 1, 0 }, 0))
			texture = nullptr;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* wpath = static_cast<const wchar_t*>(payload->Data);
				std::filesystem::path path(wpath);
				if (path.extension() == ".png")
					texture = AssetsManager::GetTexture(path.string());

				ImGui::EndDragDropTarget();
			}
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::Spacing();
	}

	bool EditorGUI::Button(const std::string& label, bool enabled)
	{
		if (enabled)
			return ImGui::Button(label.c_str(), { ImGui::GetContentRegionAvail().x, 0 });
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f });

			ImGui::Button(label.c_str(), { ImGui::GetContentRegionAvail().x, 0 });

			ImGui::PopStyleColor(3);

			return false;
		}
	}
}
