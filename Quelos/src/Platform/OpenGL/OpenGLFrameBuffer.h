#pragma once

#include "Quelos/Renderer/FrameBuffer.h"

namespace Quelos
{
	class OpenGLFrameBuffer
		: public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer() override;

		const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
		{
			QS_CORE_ASSERT(index < m_ColorAttachments.size(), "index < m_ColorAttachments.size()")
			return m_ColorAttachments[index];
		}

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void ClearColorAttachment(uint32_t attachmentIndex, int value) override;

		void Invalidate();

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void Bind() override;
		virtual void Unbind() override;
	private:
		uint32_t m_RendererID = 0;
		FrameBufferSpecification m_Specification;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecs;
		FrameBufferTextureSpecification m_DepthAttachmentSpec = FrameBufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment;
	};
}
