#pragma once

#include "Windows/SceneHierarchyPanal.h"
#include "Windows/ContentBrowserPanel.h"
#include "Windows/ConsolePanal.h"
#include "Windows/InspectorPanel.h"
#include "Windows/SpriteEditorWindow.h"
#include "Quelos/Event/KeyEvent.h"
#include <Quelos.h>
#include <Quelos/Renderer/EditorCamera.h>
#include "Quelos/Renderer/UniformBuffer.h"

#include "Lighting/LightingLayer.h"

namespace Quelos
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;

		void OnUpdate(TimeStep ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void NewProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();
		void OnScenePause();

		void OnSimulationStart();

		void OnDuplicateEntity();

		void OnOverlayRender();

		// UI Panels
		void UI_Toolbar();
	private:
		Ref<FrameBuffer> m_FrameBuffer;
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStop, m_IconStep, m_IconSimStart, m_IconSimStop, m_Container;

		EditorCamera m_EditorCamera;

		Ref<Scene> m_ActiveScene, m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity, m_CameraEntity, m_SecondCamera, m_HoveredEntity;

		Vector2 m_ViewportSize;
		Vector2 m_ViewportBounds[2];
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		int m_GizmoType = -1;

		bool m_ShowPhysicsColliders = false;

		// Panels
		InspectorPanel m_InspectorPanel;
		SceneHierarchyPanal m_HierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		ConsolePanal m_ConsolePanel;
		SpriteEditorWindow m_SpriteEditorWindow;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};

		SceneState m_SceneState = SceneState::Edit;
		bool m_ScenePaused = false;
		bool m_SceneStep = false;

		//Lighting::Lighting m_Lighting;
	};
}
