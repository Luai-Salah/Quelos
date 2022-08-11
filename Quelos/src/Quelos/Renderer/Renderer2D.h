#pragma once

#include "Quelos/Renderer/OrthographicCamera.h"
#include "Quelos/Renderer/Camera.h"
#include "Quelos/Renderer/Texture.h"
#include "Quelos/Renderer/SubTexture2D.h"
#include "Quelos/Renderer/EditorCamera.h"
#include "Quelos/Renderer/UniformBuffer.h"

#include "Quelos/Scene/Components.h"

namespace Quelos
{
	class Renderer2D
	{
	public:
		static void Init(Ref<UniformBuffer> cameraUniformBuffer);
		static void Reload();
		static void Shutdown();

		static void BeginScene(const EditorCamera& cam);
		static void BeginScene(const Matrix4& viewProjection);
		static void BeginScene(const OrthographicCamera& cam); // TODO: Remove
		static void BeginScene(const Camera& cam, const Matrix4& transform);
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawSprite(const Matrix4& transform, const SpriteRendererComponent& src, int entityID);
		static void DrawCircle(const Matrix4& transform, const Vector4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		static void DrawLine(const Vector3& p0, const Vector3& p1, const Vector4& color, int entityID = -1);

		static void DrawRect(const Matrix4& transform, const Vector4& color, int entityID = -1);
		static void DrawRect(const Vector3& position, const Vector2& size, const Vector4& color, int entityID = -1);

		static float GetLineWidth();
		static void SetLineWidth(float width);

		static void DrawQuad(const Matrix4& transform, const Vector4& color, int entityID = -1);
		static void DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color = Vector4{1.0f});
		static void DrawQuad(const Vector3& position, const Vector2& size, const Vector4& color = Vector4{1.0f});

		static void DrawQuad(const Vector2& position, const Vector2& size, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});
		static void DrawQuad(const Vector3& position, const Vector2& size, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});
		static void DrawQuad(const Matrix4& transform, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f}, int entityID = -1);
		static void DrawQuad(const Vector2& position, const Vector2& size, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});
		static void DrawQuad(const Vector3& position, const Vector2& size, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});
		static void DrawQuad(const Matrix4& transform, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f}, int entityID = -1);

		static void DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});
		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});
		static void DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});
		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const Vector4& tintColor = Vector4{1.0f});

		static void DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Vector4& color);
		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Vector4& color);
		
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t Textures = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();
	private:
		static void StartBatch();
		static void NextBatch();
	};
}
