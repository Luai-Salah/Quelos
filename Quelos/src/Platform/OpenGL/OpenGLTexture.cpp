#include "QSPCH.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

namespace Quelos
{
	static GLenum TextureFilterToOpenGlEnum(TextureFilterMode mode, int min = 1)
	{
		switch (mode)
		{
			case Quelos::TextureFilterMode::Point: return GL_NEAREST;
			case Quelos::TextureFilterMode::Bilinear: if (min == 0) return GL_NEAREST; else return GL_LINEAR;
			case Quelos::TextureFilterMode::Tilinear: return GL_LINEAR;
		}

		QS_CORE_ERROR("Filter mode not found!");

		return GL_NEAREST;
	}

	static GLenum TextureWrapToOpenGlEnum(TextureWrapMode mode)
	{
		switch (mode)
		{
			case Quelos::TextureWrapMode::Clamp: return GL_CLAMP_TO_EDGE;
			case Quelos::TextureWrapMode::Repeat: return GL_REPEAT;
		}

		QS_CORE_ERROR("Wrap mode not found!");

		return GL_REPEAT;
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		QS_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& path, const TextureSpecification& spec)
		: m_Path(path), m_Specification(spec)
	{
		QS_PROFILE_FUNCTION();

		int width, height, channals;
		stbi_set_flip_vertically_on_load(1);
		{
			QS_PROFILE_SCOPE("stbi_load->OpenGLTexture2D::OpenGLTexture2D(const std::string&)")
				m_PixelData = stbi_load(m_Path.string().c_str(), &width, &height, &channals, 0);
		}
		QS_CORE_ASSERT(m_PixelData, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channals == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channals == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		QS_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		Apply();
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QS_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetSpecification(TextureSpecification spec)
	{
		m_Specification = spec;
		m_IsApplied = false;
	}

	void OpenGLTexture2D::SetFilterMode(TextureFilterMode mode)
	{
		m_Specification.FilterMode = mode;
		m_IsApplied = false;
	}

	void OpenGLTexture2D::SetWrapMode(TextureWrapMode mode)
	{
		m_Specification.WrapMode = mode;
		m_IsApplied = false;
	}

	void OpenGLTexture2D::Apply()
	{
		glDeleteTextures(1, &m_RendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, TextureFilterToOpenGlEnum(m_Specification.FilterMode, 1));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, TextureFilterToOpenGlEnum(m_Specification.FilterMode));

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, TextureWrapToOpenGlEnum(m_Specification.WrapMode));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, TextureWrapToOpenGlEnum(m_Specification.WrapMode));

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, m_PixelData);

		m_IsApplied = true;
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		QS_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		QS_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		QS_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}
