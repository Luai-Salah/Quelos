#pragma once

#include "Quelos/Core/Window.h"

namespace Quelos
{
	class OrthographicCamera
	{
		struct OrthographicCameraBounds
		{
			float Left, Right;
			float Bottom, Top;

			float GetWidth() { return Left - Right; }
			float GetHeight() { return Top - Bottom; }
		};

	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const Vector3& GetPosition() const { return m_Position; }
		void SetPosition(const Vector3& position) { m_Position = position; RecalculateViewMatrix(); }

		void SetRotation(float rotaion) { m_Rotation = rotaion; RecalculateViewMatrix(); }
		float GetRotation() const { return m_Rotation; }

		Vector2 ScreenToWorldPosition(Vector2 screenPosition);

		const Matrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
		const Matrix4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
	private:
		void RecalculateViewMatrix();
	private:
		Matrix4 m_ProjectionMatrix;
		Matrix4 m_ViewMatrix;
		Matrix4 m_ViewProjectionMatrix;

		Vector3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
		OrthographicCameraBounds m_Bounds;
		Window* m_Winodw;
	};
}
