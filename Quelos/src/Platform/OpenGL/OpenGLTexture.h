#pragma once

#include "Quelos\Renderer\Texture.h"
#include <glad\glad.h>

namespace Quelos
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::filesystem::path& path, const TextureSpecification& spec);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }
		virtual void SetSpecification(TextureSpecification spec) override;

		virtual const TextureFilterMode& GetFilterMode() const override { return m_Specification.FilterMode; }
		virtual void SetFilterMode(TextureFilterMode mode) override;

		virtual const TextureWrapMode& GetWrapMode() const override { return m_Specification.WrapMode; }
		virtual void SetWrapMode(TextureWrapMode mode) override;

		virtual void Apply() override;
		virtual bool IsApplied() const override { return m_IsApplied; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual const std::filesystem::path& GetPath() const override { return m_Path; }
		virtual std::string GetName() const override { return m_Path.stem().string(); }
		virtual std::string GetNameEx() const override { return m_Path.filename().string(); }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}

	private:
		std::filesystem::path m_Path;
		uint32_t m_Width, m_Height;
		unsigned char* m_PixelData;
		TextureSpecification m_Specification;
		bool m_IsApplied = true;

		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}
