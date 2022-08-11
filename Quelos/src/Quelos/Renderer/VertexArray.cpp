#include "QSPCH.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform\OpenGL\OpenGLBuffer.h"
#include "Platform\OpenGL\OpenGLVertexArray.h"

namespace Quelos
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexArray>();

		}

		QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
	}
}
