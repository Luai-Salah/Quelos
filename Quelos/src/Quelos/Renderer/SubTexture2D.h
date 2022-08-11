#pragma once

#include "Quelos/Renderer/Texture.h"

namespace Quelos
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const Vector2& min = { 0.0f, 0.0f }, const Vector2& max = { 1.0f, 1.0f });
		SubTexture2D(const SubTexture2D&) = default;

		const Ref<Texture2D> GetTexture() { return m_Texture; }
		const Vector2* GetTextureCoords() { return m_TextureCoord; }

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const Vector2& coords, const Vector2& cellSize, const Vector2& spriteSize = Vector2{ 1.0f });
	private:
		Ref<Texture2D> m_Texture;
		Vector2 m_TextureCoord[4];
	};
}
