#pragma once

#include "Quelos\Renderer\OrthographicCamera.h"

#include "Quelos\Event\ApplicationEvents.h"
#include "Quelos\Event\MouseEvents.h"

namespace Quelos
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Left - Right; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectratio, bool rotation = false);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		void SetZoomLevel(float level);
		float GetZoomLevel() const { return m_ZoomLevel; }

		const OrthographicCameraBounds& GetCameraBounds() const { return m_Bounds; }
	private:
		void CalculateView();

		bool OnMouseScrolledEvent(MouseScrolledEvent e);
		bool OnWindowResizedEvent(WindowResizedEvent e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 3.0f;
		bool m_EnableRotation;

		OrthographicCamera m_Camera;
		OrthographicCameraBounds m_Bounds;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraTransilationSpeed = 5.0f;
		float m_CameraRotation = 0.0f;
		float m_CameraRotateSpeed = 10.0f;
	};
}
