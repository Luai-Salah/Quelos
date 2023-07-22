#pragma once
#include "Quelos.h"
#include "Quelos/Renderer/FrameBuffer.h"
#include "Quelos/Renderer/Texture.h"

namespace Quelos
{
	class SpriteEditorWindow
	{
	public:
		SpriteEditorWindow();
		~SpriteEditorWindow();

		void OnUpdate(TimeStep ts);
		void OnImGuiRender();
		void DisplayTexture(const Ref<Texture2D>& texture);

		bool BlockEvents() const { return !m_WindowHovered && !m_WindowFocused; }
	private:
		Ref<Texture2D> m_TextureToDisplay;
		Ref<FrameBuffer> m_FrameBuffer;
		glm::vec2 m_ViewportSize;
		bool m_WindowFocused = false;
		bool m_WindowHovered = false;

		EditorCamera m_EditorCamera;
	};
}
