#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Quelos
{
	class Math
	{
	public:
		static float Radians(float degrees) { return degrees * (float)0.01745329251994329576923690768489; }
		static glm::vec3 Radians(glm::vec3 degrees) { return {Radians(degrees.x), Radians(degrees.y), Radians(degrees.z)}; }

		static float Degrees(float radians) { return radians / (float)0.01745329251994329576923690768489; }
		static glm::vec3 Degrees(glm::vec3 radians) { return {Degrees(radians.x), Degrees(radians.y), Degrees(radians.z)}; }

		static float Length(float x) { return abs(x); }

		static float Clamp(float value, float min, float max) { return std::min(std::max(value, min), max); }
		static float Lerp(float value1, float value2, float t) { return std::lerp(value1, value2, t); /*value1* (1.0f - t) + (value2 * t);*/ }

		static glm::mat4 Perspective(float verticalFOV, float aspectRatio, float nearClip, float farClip);

		static glm::mat4 Translate(const glm::vec3& v);
		static glm::mat4 Scale(const glm::vec3& v);
		static glm::mat4 Rotate(float angle, const glm::vec3& v);
		static glm::vec3 Rotate(const glm::quat& q, const glm::vec3& v);

		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale);

		template<typename T>
		static T Epsilon() { T result = std::numeric_limits<T>().epsilon(); return result; }
		static bool EpsilonEqual(float const& x, float const& y, float const& epsilon) { return abs(x - y) < epsilon; }
		static bool EpsilonNotEqual(float const& x, float const& y, float const& epsilon) { return abs(x - y) >= epsilon; }

		template<typename T>
		static T Pi() { return static_cast<T>(3.14159265358979323846264338327950288); }
	};
}
