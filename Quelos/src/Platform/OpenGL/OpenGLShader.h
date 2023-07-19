#pragma once

#include "Quelos\Renderer\Shader.h"

//TODO: Remove.
typedef unsigned int GLenum;

namespace Quelos
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int i) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetMatrix4(const std::string& name, const glm::mat4& m) override;
		virtual void SetVector4(const std::string& name, const glm::vec4& v) override;
		virtual void SetVector3(const std::string& name, const glm::vec3& v) override;
		virtual void SetFloat(const std::string& name, float i) override;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix4);

		void UploadUniformVector2(const std::string& name, const glm::vec2& vec2);
		void UploadUniformVector3(const std::string& name, const glm::vec3& vec3);
		void UploadUniformVector4(const std::string& name, const glm::vec4& vec4);

		void UploadUniformFloat(const std::string& name, float value);

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
	private:
		void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		void CompileOrGetOpenGLBinaries();
		void CreateProgram();
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

		std::string ReadFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;

		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
	};
}
