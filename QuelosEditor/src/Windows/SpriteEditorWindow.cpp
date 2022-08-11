#include "SpriteEditorWindow.h"
#include "QSPCH.h"

#include <imgui.h>

#include "Quelos/Renderer/Renderer2D.h"

namespace Quelos
{
	SpriteEditorWindow::SpriteEditorWindow()
	{
		FrameBufferSpecification fbSpec;
		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8 };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;

		m_FrameBuffer = FrameBuffer::Create(fbSpec);
		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_EditorCamera.SetDistance(3.0f);
		m_EditorCamera.SetLockRotation(true);
	}

	SpriteEditorWindow::~SpriteEditorWindow()
	{
	}

	void SpriteEditorWindow::OnUpdate(TimeStep ts)
	{
		m_FrameBuffer->Bind();

		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		if (m_WindowFocused)
			m_EditorCamera.OnUpdate(ts);
		Renderer2D::BeginScene(m_EditorCamera);
		if (m_TextureToDisplay)
		Renderer2D::DrawQuad(Vector3(0.0f), Vector2(1.0f), m_TextureToDisplay);

		Renderer2D::EndScene();

		m_FrameBuffer->Unbind();
	}

	void SpriteEditorWindow::OnImGuiRender()
	{
		if (ImGui::Begin("Sprite Editor"))
		{
			ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();

			if (m_ViewportSize != *reinterpret_cast<Vector2*>(&viewPortPanelSize))
			{
				m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };
				m_FrameBuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
				m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			}

			if (m_TextureToDisplay)
				ImGui::Image((ImTextureID)m_FrameBuffer->GetColorAttachmentRendererID(), 
							 { m_ViewportSize.x, m_ViewportSize.y}, { 0, 1 }, { 1, 0 });

			m_WindowFocused = ImGui::IsWindowFocused();
			m_WindowHovered = ImGui::IsWindowHovered();

			ImGui::End();
		}
		else ImGui::End();
	}

	void SpriteEditorWindow::DisplayTexture(const Ref<Texture2D>& texture)
	{
		m_TextureToDisplay = texture;
	}
}
