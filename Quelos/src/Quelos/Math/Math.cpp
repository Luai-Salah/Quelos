#include "QSPCH.h"
#include "Quelos/Math/Math.h"

#include <glm/gtx/matrix_decompose.inl>
#include <glm/gtc/quaternion.hpp>

namespace Quelos
{
	Matrix4 Math::Perspective(float verticalFOV, float aspectRatio, float nearClip, float farClip)
	{
		float const tanHalfFovy = tan(verticalFOV / 2.0f);

		Matrix4 Result(0.0f);
		Result[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
		Result[1][1] = 1.0f / (tanHalfFovy);
		Result[2][2] = -(farClip + nearClip) / (farClip - nearClip);
		Result[2][3] = -1.0f;
		Result[3][2] = (-2.0f * farClip * nearClip) / (farClip - nearClip);
		return Result;
	}

	Matrix4 Math::Translate(const Vector3& v)
	{
		Matrix4 m(1.0f);
		Matrix4 Result(m);
		Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
		return Result;
	}

	Matrix4 Math::Scale(const Vector3& v)
	{
		Matrix4 m(1.0f);
		Matrix4 Result;
		Result[0] = m[0] * v[0];
		Result[1] = m[1] * v[1];
		Result[2] = m[2] * v[2];
		Result[3] = m[3];
		return Result;
	}

	Matrix4 Math::Rotate(float angle, const Vector3& v)
	{
		Matrix4 m(1.0f);
		float const a = angle;
		float const c = cos(a);
		float const s = sin(a);

		Vector3 axis(v);
		Vector3 temp((float(1) - c) * axis);

		Matrix4 Rotate;
		Rotate[0][0] = c + temp[0] * axis[0];
		Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = c + temp[1] * axis[1];
		Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = c + temp[2] * axis[2];

		Matrix4 Result;
		Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
		Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
		Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
		Result[3] = m[3];
		return Result;
	}

	Vector3 Math::Rotate(const Quaternion& q, const Vector3& v)
	{
		return q * v;
	}

	bool Math::DecomposeTransform(const Matrix4& transform, Vector3& outPosition, Vector3& outRotation, Vector3& outScale)
	{
		using T = float;

		Matrix4 LocalMatrix(transform);

		// Normalize the matrix.
		if (EpsilonEqual(LocalMatrix[3][3], static_cast<float>(0), Epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			EpsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), Epsilon<T>()) ||
			EpsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), Epsilon<T>()) ||
			EpsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), Epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		outPosition = Vector3(LocalMatrix[3]);
		LocalMatrix[3] = Vector4(0.0f, 0.0f, 0.0f, LocalMatrix[3].w);

		Vector3 Row[3], Pdum3;

		// Now get scale and shear.
		for (uint32_t i = 0; i < 3; ++i)
			for (uint32_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];
		
		// Compute X scale factor and normalize first row.
		outScale.x = glm::length(Row[0]);
		Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
		outScale.y = glm::length(Row[1]);
		Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
		outScale.z = glm::length(Row[2]);
		Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = Vector3::Cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (Vector3::Dot(Row[0], Pdum3) < Vector3())
		{
			for (uint32_t i = 0; i < 3; i++)
			{
				outScale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif
		outRotation.y = asin(-Row[0][2]);
		if (cos(outRotation.y) != 0)
		{
			outRotation.x = atan2(Row[1][2], Row[2][2]);
			outRotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			outRotation.x = atan2(-Row[2][0], Row[1][1]);
			outRotation.z = 0;
		}

		return true;
	}
}
