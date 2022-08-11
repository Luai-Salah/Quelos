#include "QSPCH.h"
#include "SubTexture2D.h"

#include "Quelos/Renderer/Texture.h"

namespace Quelos
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const Vector2& min, const Vector2& max)
		: m_Texture(texture)
	{
		m_TextureCoord[0] = { min.x, min.y };
		m_TextureCoord[1] = { max.x, min.y };
		m_TextureCoord[2] = { max.x, max.y };
		m_TextureCoord[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const Vector2& coords, const Vector2& cellSize, const Vector2& spriteSize)
	{
		Vector2 min = { (coords.x * cellSize.x) / texture->GetWidth(), (coords.y * cellSize.y) / texture->GetHeight() };
		Vector2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight() };

		Ref<SubTexture2D> result = CreateRef<SubTexture2D>(texture, min, max);
		return result;
	}
}
