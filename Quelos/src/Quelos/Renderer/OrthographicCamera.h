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

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		void SetRotation(float rotaion) { m_Rotation = rotaion; RecalculateViewMatrix(); }
		float GetRotation() const { return m_Rotation; }

		glm::vec2 ScreenToWorldPosition(glm::vec2 screenPosition);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		static glm::mat4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
		OrthographicCameraBounds m_Bounds;
		Window* m_Winodw;
	};
}
