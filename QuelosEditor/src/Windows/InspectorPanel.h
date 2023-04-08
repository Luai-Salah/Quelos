#pragma once

#include "Quelos/Scene/ScriptableEntity.h"
#include "Quelos/Core/Input.h"

namespace Quelos
{
	class InspectorPanel
	{
	public:
		InspectorPanel();
		~InspectorPanel();
		void SetContext(const Ref<Scene>& contex);

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);

		Ref<Texture2D> GetSelectedTexture() const { return m_SelectedTexture; }
		void SetSelectedTexture(const Ref<Texture2D>& texture);

		void OnImGuiRender();
	public:
		static InspectorPanel* Get() { return s_Instance; }
	private:
		void DrawComponents(Entity entity);
		void DrawTextureFields(const Ref<Texture2D>& texture);

		template<typename Component>
		void DrawAddComponent(const std::string& name);
	private:
		static InspectorPanel* s_Instance;
	private:
		Entity m_SelectionContext;
		Ref<Texture2D> m_SelectedTexture;
		Ref<Scene> m_Context;
	};
}
