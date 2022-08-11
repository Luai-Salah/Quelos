#pragma once

#include <Quelos.h>
#include "InspectorPanel.h"

namespace Quelos
{
	class SceneHierarchyPanal
	{
	public:
		SceneHierarchyPanal() = default;
		SceneHierarchyPanal(const Ref<Scene>& context, InspectorPanel& inspector);

		void SetContext(const Ref<Scene>& context);
		void SetInspector(InspectorPanel& inspector) { m_Inspector = inspector; }

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;
		InspectorPanel& m_Inspector;
	};
}
