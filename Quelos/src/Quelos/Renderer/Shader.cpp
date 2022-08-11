#include "QSPCH.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform\OpenGL\OpenGLShader.h"

namespace Quelos
{
	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(filePath);
		}

		QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);
		}

		QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
	}
}
