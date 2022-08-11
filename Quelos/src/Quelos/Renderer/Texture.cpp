#include "QSPCH.h"
#include "Texture.h"

#include "Platform\OpenGL\OpenGLTexture.h"
#include "Renderer.h"

namespace Quelos
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(width, height);
		}

		QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path, const TextureSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(path, spec);
		}

		QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
	}
}
