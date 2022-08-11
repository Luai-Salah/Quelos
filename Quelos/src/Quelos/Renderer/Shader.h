#pragma once

namespace Quelos
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int i) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float i) = 0;
		virtual void SetMatrix4(const std::string& name, const Matrix4& m) = 0;
		virtual void SetVector4(const std::string& name, const Vector4& v) = 0;
		virtual void SetVector3(const std::string& name, const Vector3& v) = 0;

		static Ref<Shader> Create(const std::string& filePath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	};
}
