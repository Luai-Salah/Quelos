#pragma once

#include "Quelos/Renderer/UniformBuffer.h"

namespace Quelos
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		~OpenGLUniformBuffer() override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t m_RendererID;
	};
}
