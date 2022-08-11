#pragma once

#include "Camera.h"
#include "Quelos/Core/Timestep.h"
#include "Quelos/Event/Event.h"
#include "Quelos/Event/MouseEvents.h"

namespace Quelos
{

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		[[nodiscard]] inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		bool IsLockRotation() const { return m_LockRotation; }
		void SetLockRotation(bool value) { m_LockRotation = value; }

		[[nodiscard]] const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]] Matrix4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		[[nodiscard]] Vector3 GetUpDirection() const;
		[[nodiscard]] Vector3 GetRightDirection() const;
		[[nodiscard]] Vector3 GetForwardDirection() const;
		[[nodiscard]] const Vector3& GetPosition() const { return m_Position; }
		[[nodiscard]] Quaternion GetOrientation() const;

		[[nodiscard]] float GetPitch() const { return m_Pitch; }
		[[nodiscard]] float GetYaw() const { return m_Yaw; }
	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const Vector2& delta);
		void MouseRotate(const Vector2& delta);
		void MouseZoom(float delta);
		void PlayerMove(const Vector2& delta);

		[[nodiscard]] Vector3 CalculatePosition() const;

		[[nodiscard]] Vector2 PanSpeed() const;
		[[nodiscard]] float RotationSpeed() const;
		[[nodiscard]] float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		Matrix4 m_ViewMatrix;
		Vector3 m_Position = { 0.0f, 0.0f, 0.0f };
		Vector3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		Vector2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
		bool m_PlayerMode = false, m_LockRotation = false;
	};

}
