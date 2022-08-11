#include "QSPCH.h"
#include "FrameBuffer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Quelos/Renderer/Renderer.h"

namespace Quelos
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		QS_CORE_ERROR("RendererAPI::None currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return  CreateRef<OpenGLFrameBuffer>(spec);

			default: QS_CORE_ERROR("Unknown RendererAPI supported!"); return nullptr;
		}
	}
}
