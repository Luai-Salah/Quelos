#include "QSPCH.h"
#include "InspectorPanel.h"

#include "Quelos/Core/Input.h"
#include "Quelos/Scene/ScriptableEntity.h"
#include "Quelos/Scripting/ScriptEngine.h"

#include "..\Gui\EditorGui.h"

#include <imgui.h>

namespace Quelos
{
	InspectorPanel* InspectorPanel::s_Instance;

	class PlayerController : public ScriptableEntity
	{
	public:
		float m_Speed = 5000.0f;

		void OnUpdate(TimeStep ts)
		{
			auto& rb = GetComponent<Rigidbody2DComponent>();

			if (Input::GetKey(KeyCode::A))
				rb.ApplyForceToCenter(Vector3(-1.0f, 0.0f, 0.0f) * (m_Speed * ts));
			if (Input::GetKey(KeyCode::D))
				rb.ApplyForceToCenter(Vector3(1.0f, 0.0f, 0.0f) * (m_Speed * ts));
		}
	};

	InspectorPanel::InspectorPanel()
	{
		s_Instance = this;
	}

	InspectorPanel::~InspectorPanel()
	{
	}

	void InspectorPanel::SetContext(const Ref<Scene>& contex)
	{
		m_Context = contex;
	}

	void InspectorPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedTexture = nullptr;
		m_SelectionContext = entity;
	}
	void InspectorPanel::SetSelectedTexture(const Ref<Texture2D>& texture)
	{
		m_SelectionContext = {};
		m_SelectedTexture = texture;
	}

	void InspectorPanel::OnImGuiRender()
	{
		static bool show_inspector = true;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::MenuItem("Inspector", nullptr, &show_inspector);
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (show_inspector)
		{
			if (ImGui::Begin("Inspector", &show_inspector))
			{
				if (m_SelectionContext)
					DrawComponents(m_SelectionContext);
				else if (m_SelectedTexture)
					DrawTextureFields(m_SelectedTexture);

				ImGui::End();
			}
			else ImGui::End();
		}
	}

	template <typename Component>
	std::function<void()> Remove(Entity e) { return [&]() { e.RemoveComponent<Component>(); }; }

	template<typename Component>
	void InspectorPanel::DrawAddComponent(const std::string& name)
	{
		if (!m_SelectionContext.HasComponent<Component>())
		{
			if (ImGui::MenuItem(name.c_str()))
			{
				m_SelectionContext.AddComponent<Component>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void InspectorPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tc = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tc.c_str());
			if (ImGui::InputText("##tag", buffer, sizeof(buffer)))
			{
				if (Input::GetKeyDown(KeyCode::Enter))
					tc = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component", { ImGui::GetContentRegionAvail().x, 0.0f }))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DrawAddComponent<CameraComponent>("Camera");
			DrawAddComponent<ScriptComponent>("Script");
			DrawAddComponent<SpriteRendererComponent>("SpriteRenderer");
			DrawAddComponent<CircleRendererComponent>("CircleRenderer");
			DrawAddComponent<Rigidbody2DComponent>("Rigidbody2D");
			DrawAddComponent<BoxCollider2DComponent>("BoxCollider2D");
			DrawAddComponent<CircleCollider2DComponent>("CircleCollider2D");
			DrawAddComponent<CameraComponent>("CircleCollider2D");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		ImGui::Separator();


		EditorGui::Component<TransformComponent>("Camera", entity, [](Entity entity, TransformComponent& transform)
		{
			EditorGui::InputVector3("Position", transform.Position, 0.0f, 100.0f);

			Vector3 rotation = Math::Degrees(transform.Rotation);
			EditorGui::InputVector3("Rotation", rotation, 0.0f, 100.0f);
			transform.Rotation = Math::Radians(rotation);

			EditorGui::InputVector3("Scale", transform.Scale, 1.0f, 100.0f);
		}, false);

		EditorGui::Component<CameraComponent>("Camera", entity, [](Entity entity, CameraComponent& cameraComponent)
		{
			auto& camera = cameraComponent.Camera;

			const char* projectionTypeString[] = { "Perspective", "Orthographic" };

			EditorGui::CheckBox("Primary", cameraComponent.Primary);
			EditorGui::EntitySlot("Target", cameraComponent.TargetID);

			const char* currentProjectionTypeString = projectionTypeString[static_cast<int>(camera.GetProjectionType())];
			if (EditorGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeString[i];
					if (ImGui::Selectable(projectionTypeString[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeString[i];
						camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				EditorGui::EndCombo();
			}
			ImGui::Columns(1);

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float persSize = Math::Degrees(camera.GetPerspectiveVerticalFOV());
				if (EditorGui::InputFloat("Vertical FOV", persSize))
					camera.SetPerspectiveVerticalFOV(Math::Radians(persSize));

				float persNear = camera.GetPerspectiveNearClip();
				if (EditorGui::InputFloat("Near Clip", persNear))
					camera.SetPerspectiveNearClip(persNear);

				float persFar = camera.GetPerspectiveFarClip();
				if (EditorGui::InputFloat("Far Clip", persFar))
					camera.SetPerspectiveFarClip(persFar);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (EditorGui::InputFloat("Size", orthoSize))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (EditorGui::InputFloat("Near Clip", orthoNear))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (EditorGui::InputFloat("Far Clip", orthoFar))
					camera.SetOrthographicFarClip(orthoFar);

				EditorGui::CheckBox("Fixed Aspect Ration", cameraComponent.FixedAspectRatio);
			}
		});

		EditorGui::Component<ScriptComponent>("Script", entity, [scene = m_Context](Entity entity, ScriptComponent& sc) mutable
		{
			bool scriptExists = ScriptEngine::EntityClassExists(sc.ClassName);

			static char buffer[64];
			strcpy_s(buffer, sizeof(buffer), sc.ClassName.c_str());

			if (!scriptExists)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
			if (ImGui::InputText("Class", buffer, sizeof(buffer)))
				sc.ClassName = buffer;

			// Fields
			if (scene->IsRunning())
			{
				Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity);
				if (scriptInstance)
				{
					const auto& fields = scriptInstance->GetScriptClass()->GetFields();
					for (const auto& [name, field] : fields)
					{
						if (field.Type == ScriptFieldType::Float)
						{
							float data = scriptInstance->GetFieldValue<float>(name);
							if (EditorGui::InputFloat(name, data))
								scriptInstance->SetFieldValue(name, data);
						}
					}
				}
			}
			else
			{
				if (scriptExists)
				{
					Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
					const auto& fields = entityClass->GetFields();

					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					for (const auto& [name, field] : fields)
					{
						if (entityFields.find(name) != entityFields.end())
						{
							ScriptFieldInstance& scriptField = entityFields.at(name);
							
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptField.GetValue<float>();
								if (EditorGui::InputFloat(name, data))
									scriptField.SetValue(data);
							}
						}
						else
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = 0;
								if (EditorGui::InputFloat(name, data))
								{
									ScriptFieldInstance& instance = entityFields[name];

									instance.Field = field;
									instance.SetValue(data);
								}
							}
						}
					}
				}
			}

			if (!scriptExists)
				ImGui::PopStyleColor();
		});

		EditorGui::Component<CircleRendererComponent>("Circle Renderer", entity, [](Entity entity, CircleRendererComponent& cr)
		{
			EditorGui::InputColor4("Color", cr.Color);
			EditorGui::InputFloat("Thickness", cr.Thickness, 0.025f, 0.0f, 1.0f);
			EditorGui::InputFloat("Fade", cr.Fade, 0.00025f, 0.0f, 1.0f);
		});

		EditorGui::Component<SpriteRendererComponent>("Sprite Renderer", entity, [](Entity entity, SpriteRendererComponent& spriteRenderer)
		{
			EditorGui::InputColor4("Color", spriteRenderer.Color);

			EditorGui::TextureSlot("Texture", spriteRenderer.Texture);

			EditorGui::InputFloat("Tiling Factor", spriteRenderer.TilingFactor, 0.1f, 0.0f, 100.0f);

			bool enable;
			EditorGui::CheckBox("SubTexture", enable);
			if (enable)
			{
				if (EditorGui::InputVector2("Min Texture Coords", spriteRenderer.MinTextureCoords, 0.0f, 0.01f, 0.0f, 1.0f))
				{
					spriteRenderer.Texture = CreateRef<SubTexture2D>(spriteRenderer.Texture->GetTexture(), spriteRenderer.MinTextureCoords,
																	 spriteRenderer.MaxTextureCoords);
				}

				if (EditorGui::InputVector2("Max Texture Coords", spriteRenderer.MaxTextureCoords, 1.0f, 0.01f, 0.0f, 1.0f))
				{
					spriteRenderer.Texture = CreateRef<SubTexture2D>(spriteRenderer.Texture->GetTexture(), spriteRenderer.MinTextureCoords,
																	 spriteRenderer.MaxTextureCoords);
				}
			}
		});

		EditorGui::Component<Rigidbody2DComponent>("Rigidbody 2D", entity, [](Entity entity, Rigidbody2DComponent& rb)
		{
			const char* rbTypeSettings[] = { "Static", "Dynamic", "Kinematic" };

			const char* currentRbTypeSettings = rbTypeSettings[static_cast<int>(rb.Type)];
			if (EditorGui::BeginCombo("Body Type", currentRbTypeSettings))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentRbTypeSettings == rbTypeSettings[i];
					if (ImGui::Selectable(rbTypeSettings[i], isSelected))
					{
						currentRbTypeSettings = rbTypeSettings[i];
						rb.Type = static_cast<Rigidbody2DComponent::BodyType>(i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				EditorGui::EndCombo();
			}

			ImGui::Columns(1);

			EditorGui::CheckBox("Fixed Rotation", rb.FixedRotation);
		});

		EditorGui::Component<BoxCollider2DComponent>("Box Collider 2D", entity, [](Entity entity, BoxCollider2DComponent& boxCollider)
		{
			EditorGui::InputVector2("Offset", boxCollider.Offset);// ImGui::DragFloat2("Offset", boxCollider.Offset, 0.01f);
			EditorGui::InputVector2("Size", boxCollider.Size, 0.5f);

			EditorGui::InputFloat("Density", boxCollider.Density, 0.01f, 0.0f, 1.0f);
			EditorGui::InputFloat("Friction", boxCollider.Friction, 0.01f, 0.0f, 1.0f);
			EditorGui::InputFloat("Restitution", boxCollider.Restitution, 0.01f, 0.0f, 1.0f);
			EditorGui::InputFloat("RestitutionThreshold", boxCollider.RestitutionThreshold, 0.1f, 0.0f);
		});

		EditorGui::Component<CircleCollider2DComponent>("Circle Collider 2D", entity, [](Entity entity, CircleCollider2DComponent& cc)
		{
			EditorGui::InputVector2("Offset", cc.Offset);
			EditorGui::InputFloat("Radius", cc.Radius, 0.5f);

			EditorGui::InputFloat("Density", cc.Density, 0.01f, 0.0f, 1.0f);
			EditorGui::InputFloat("Friction", cc.Friction, 0.01f, 0.0f, 1.0f);
			EditorGui::InputFloat("Restitution", cc.Restitution, 0.01f, 0.0f, 1.0f);
			EditorGui::InputFloat("RestitutionThreshold", cc.RestitutionThreshold, 0.1f, 0.0f);
		});

		EditorGui::Component<NativeScriptComponent>("Player Controller", entity, [](Entity entity, NativeScriptComponent& ns)
		{
			PlayerController* pc = static_cast<PlayerController*>(ns.Instance);

			if (pc)
				EditorGui::InputFloat("Speed", pc->m_Speed, 0.5f);
		});
	}

	void InspectorPanel::DrawTextureFields(const Ref<Texture2D>& texture)
	{
		auto& texSpec = texture->GetSpecification();

		ImGui::Image((ImTextureID)texture->GetRendererID(), { 50.0f, 50.0f }, { 0, 1 }, { 1, 0 });
		ImGui::SameLine();
		ImGui::Text(texture->GetNameEx().c_str());
		ImGui::Separator();

		const char* filterModeSetting[] = { "Point", "Bilinear", "Tilinear" };

		EditorGui::Combo("Filter Mode", filterModeSetting, 3, static_cast<int>(texSpec.FilterMode), [&](int selectedIndex)
		{
			texture->SetFilterMode(static_cast<TextureFilterMode>(selectedIndex));
		});

		const char* wrapModeSetting[] = { "Clamp", "Repeat" };

		EditorGui::Combo("Wrap Mode", wrapModeSetting, 2, static_cast<int>(texSpec.WrapMode), [&](int selectedIndex)
		{
			texture->SetWrapMode(static_cast<TextureWrapMode>(selectedIndex));
		});

		if (EditorGui::Button("Apply and Save", !texture->IsApplied()))
		{
			texture->Apply();
			AssetsManager::SerializeAsset(texture);
		}
	}
}
