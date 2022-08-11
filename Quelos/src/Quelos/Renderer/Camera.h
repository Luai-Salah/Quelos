#pragma once

namespace Quelos
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const Matrix4& projection) : m_Projection(projection) {}

		virtual ~Camera() = default;

		const Matrix4& GetProjection() const { return m_Projection; }
	protected:
		Matrix4 m_Projection = Matrix4(1.0f);
	};
}
