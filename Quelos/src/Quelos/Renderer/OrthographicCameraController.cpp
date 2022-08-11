#include "QSPCH.h"
#include "OrthographicCameraController.h"
#include "Quelos\Core\Input.h"

namespace Quelos
{
	OrthographicCameraController::OrthographicCameraController(float aspectratio, bool rotation)
		: m_AspectRatio(aspectratio), m_EnableRotation(rotation),
		m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel })
	{
	}

	void OrthographicCameraController::OnUpdate(TimeStep ts)
	{
		QS_PROFILE_FUNCTION();

		if (Input::GetKey(KeyCode::W))
			m_CameraPosition.y += m_ZoomLevel * ts;

		if (Input::GetKey(KeyCode::S))
			m_CameraPosition.y -= m_ZoomLevel * ts;

		if (Input::GetKey(KeyCode::D))
			m_CameraPosition.x += m_ZoomLevel * ts;

		if (Input::GetKey(KeyCode::A))
			m_CameraPosition.x -= m_ZoomLevel * ts;

		if (m_EnableRotation)
		{
			if (Input::GetKey(KeyCode::Q))
				m_CameraRotation += m_CameraRotateSpeed * ts;
			if (Input::GetKey(KeyCode::E))
				m_CameraRotation -= m_CameraRotateSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);
	}

	void OrthographicCameraController::SetZoomLevel(float level)
	{
		m_ZoomLevel = level;
		m_ZoomLevel = Math::Clamp(m_ZoomLevel, 0.25f, 10.0f);
		CalculateView();
	}

	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		QS_PROFILE_FUNCTION();
		
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVNT_FUNC(OrthographicCameraController::OnMouseScrolledEvent));
		dispatcher.Dispatch<WindowResizedEvent>(BIND_EVNT_FUNC(OrthographicCameraController::OnWindowResizedEvent));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		CalculateView();
	}

	bool OrthographicCameraController::OnMouseScrolledEvent(MouseScrolledEvent e)
	{
		QS_PROFILE_FUNCTION();
		
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = Math::Clamp(m_ZoomLevel, 0.25f, 10.0f);
		CalculateView();

		return false;
	}

	bool OrthographicCameraController::OnWindowResizedEvent(WindowResizedEvent e)
	{
		QS_PROFILE_FUNCTION();
		
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		CalculateView();

		return false;
	}
}
