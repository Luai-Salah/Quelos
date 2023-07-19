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
		static void BeginScene(const glm::mat4& viewProjection);
		static void BeginScene(const OrthographicCamera& cam); // TODO: Remove
		static void BeginScene(const Camera& cam, const glm::mat4& transform);
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, int entityID);
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);

		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);

		static float GetLineWidth();
		static void SetLineWidth(float width);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = glm::vec4{1.0f});
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4{1.0f});

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f}, int entityID = -1);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});
		static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f}, int entityID = -1);

		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& tex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTex, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4{1.0f});

		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		
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
