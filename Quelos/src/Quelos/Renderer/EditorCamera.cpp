#include "QSPCH.h"
#include "EditorCamera.h"

#include "Quelos/Core/Input.h"
#include "Quelos/Core/KeyCodes.h"
#include "Quelos/Core/MouseButtonCodes.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Quelos
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
		Camera(Math::Perspective(Math::Radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
		MousePan(Vector2{ 0.01f });
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(Math::Radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		if (m_LockRotation)
			m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation

		m_Position = CalculatePosition();

		const Quaternion orientation = GetOrientation();
		m_ViewMatrix = glm::translate(Matrix4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	Vector2 EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::OnUpdate(TimeStep ts)
	{
		if (Input::GetKey(KeyCode::LeftControl))
		{
			const Vector2& mouse = Input::GetMousePosition();
			const Vector2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(MouseButtonCode::Left))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(MouseButtonCode::Right))
				MouseZoom(delta.y);
		}
		else
		{
			const Vector2& mouse = Input::GetMousePosition();
			const Vector2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(MouseButtonCode::Right))
				MouseRotate(delta);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVNT_FUNC(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const Vector2& delta)
	{
		Vector2 speed = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * speed.x * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * speed.y * m_Distance;
	}

	void EditorCamera::MouseRotate(const Vector2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	void EditorCamera::PlayerMove(const Vector2& delta)
	{
		m_PlayerMode = true;
		m_Pitch += delta.y;
	}

	Vector3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), Vector3(0.0f, 1.0f, 0.0f));
	}

	Vector3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), Vector3(1.0f, 0.0f, 0.0f));
	}

	Vector3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), Vector3(0.0f, 0.0f, -1.0f));
	}

	Vector3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	Quaternion EditorCamera::GetOrientation() const
	{
		return Quaternion(Vector3(-m_Pitch, -m_Yaw, 0.0f));
	}

}
