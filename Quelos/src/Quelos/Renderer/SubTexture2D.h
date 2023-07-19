#pragma once

#include "Quelos/Renderer/Texture.h"

namespace Quelos
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min = { 0.0f, 0.0f }, const glm::vec2& max = { 1.0f, 1.0f });
		SubTexture2D(const SubTexture2D&) = default;

		const Ref<Texture2D> GetTexture() { return m_Texture; }
		const glm::vec2* GetTextureCoords() { return m_TextureCoord; }

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = glm::vec2{ 1.0f });
	private:
		Ref<Texture2D> m_Texture;
		glm::vec2 m_TextureCoord[4];
	};
}
