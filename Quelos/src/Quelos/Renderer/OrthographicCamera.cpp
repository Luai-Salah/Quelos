#include "QSPCH.h"
#include "OrthographicCamera.h"

#include "Quelos/Core/Application.h"
#include "Quelos/Core/Input.h"

namespace Quelos
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(Orthographic(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f),
		m_Bounds({ left, right, bottom, top }), m_Winodw(&Application::GetWindow())
	{
		QS_PROFILE_FUNCTION();

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		QS_PROFILE_FUNCTION();

		m_Bounds = { left, right, bottom, top };
		m_ProjectionMatrix = Orthographic(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	Vector2 OrthographicCamera::ScreenToWorldPosition(Vector2 screenPosition)
	{
		auto width = m_Winodw->GetWidth();
		auto height = m_Winodw->GetHeight();

		auto pos = m_Position;
		screenPosition.x = (screenPosition.x / width) * m_Bounds.GetWidth() - m_Bounds.GetWidth() * 0.5f;
		screenPosition.y = m_Bounds.GetHeight() * 0.5f - (screenPosition.y / height) * m_Bounds.GetHeight();
		return { -screenPosition.x + pos.x, screenPosition.y + pos.y };
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		QS_PROFILE_FUNCTION();

		Matrix4 transform = glm::translate(Matrix4(1.0f), m_Position) *
			glm::rotate(Matrix4(1.0f), Math::Radians(m_Rotation), Vector3(0.0f, 0.0f, 1.0f));

		m_ViewMatrix = glm::inverse(transform);

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	Matrix4 OrthographicCamera::Orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		Matrix4 Result(1);
		Result[0][0] = static_cast<float>(2) / (right - left);
		Result[1][1] = static_cast<float>(2) / (top - bottom);
		Result[2][2] = -static_cast<float>(2) / (zFar - zNear);
		Result[3][0] = -(right + left) / (right - left);
		Result[3][1] = -(top + bottom) / (top - bottom);
		Result[3][2] = -(zFar + zNear) / (zFar - zNear);
		return Result;
	}
}
