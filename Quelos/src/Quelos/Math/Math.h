#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;
typedef glm::mat4 Matrix4;
typedef glm::quat Quaternion;

namespace Quelos
{
	class Math
	{
	public:
		static float Radians(float degrees) { return degrees * (float)0.01745329251994329576923690768489; }
		static Vector3 Radians(Vector3 degrees) { return {Radians(degrees.x), Radians(degrees.y), Radians(degrees.z)}; }

		static float Degrees(float radians) { return radians / (float)0.01745329251994329576923690768489; }
		static Vector3 Degrees(Vector3 radians) { return {Degrees(radians.x), Degrees(radians.y), Degrees(radians.z)}; }

		static float Length(float x) { return abs(x); }

		static float Clamp(float value, float min, float max) { return std::min(std::max(value, min), max); }
		static float Lerp(float value1, float value2, float t) { return std::lerp(value1, value2, t); /*value1* (1.0f - t) + (value2 * t);*/ }

		static Matrix4 Perspective(float verticalFOV, float aspectRatio, float nearClip, float farClip);

		static Matrix4 Translate(const Vector3& v);
		static Matrix4 Scale(const Vector3& v);
		static Matrix4 Rotate(float angle, const Vector3& v);
		static Vector3 Rotate(const Quaternion& q, const Vector3& v);

		static bool DecomposeTransform(const Matrix4& transform, Vector3& outPosition, Vector3& outRotation, Vector3& outScale);

		template<typename T>
		static T Epsilon() { T result = std::numeric_limits<T>().epsilon(); return result; }
		static bool EpsilonEqual(float const& x, float const& y, float const& epsilon) { return abs(x - y) < epsilon; }
		static bool EpsilonNotEqual(float const& x, float const& y, float const& epsilon) { return abs(x - y) >= epsilon; }

		template<typename T>
		static T Pi() { return static_cast<T>(3.14159265358979323846264338327950288); }
	};
}
