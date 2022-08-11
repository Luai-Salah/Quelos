#include "QSPCH.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform\OpenGL\OpenGLBuffer.h"

namespace Quelos
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return  CreateRef<OpenGLVertexBuffer>(size);

			default: QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return  CreateRef<OpenGLVertexBuffer>(vertices, size);

			default: QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
		}
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return  CreateRef<OpenGLIndexBuffer>(indices, count);

			default: QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
		}
	}
}
