﻿#include "QSPCH.h"
#include "Quelos/Renderer/Renderer2D.h"

#include "Renderer.h"
#include "Quelos/Renderer/VertexArray.h"
#include "Quelos/Renderer/Shader.h"
#include "Quelos/Renderer/RenderCommand.h"
#include "Quelos/Renderer/UniformBuffer.h"

namespace Quelos
{
	enum class RendererSceneState{ Began, Ended };

	struct QuadVertex
	{
		Vector3 Position;
		Vector4 Color;
		Vector2 TexCoord;
		float TexIndex = 0.0f;
		float TilingFactor = 1.0f;

		// Editor-Only
		int EntityID = -1;
	};

	struct LineVertex
	{
		Vector3 Position;
		Vector4 Color;

		// Editor-Only
		int EntityID = -1;
	};

	struct CircleVertex
	{
		Vector3 WorldPosition;
		Vector3 LocalPosition;
		Vector4 Color;
		float Thickness = 1.0f;
		float Fade = 0.005f;

		// Editor-Only
		int EntityID = -1;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		// Quad
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		// Circle
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		// Line
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		Vector4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			Matrix4 ViewProjection;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		RendererSceneState RendererSceneState = RendererSceneState::Ended;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init(Ref<UniformBuffer> cameraUniformBuffer)
	{
		QS_PROFILE_FUNCTION();

		s_Data = new Renderer2DData();
		s_Data->QuadVertexArray = VertexArray::Create();

		s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));
		s_Data->QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Vector3,	"a_Position"	},
			{ ShaderDataType::Vector4,	"a_Color"		},
			{ ShaderDataType::Vector2,	"a_TexCoord"	},
			{ ShaderDataType::Float,	"a_TexIndex"	},
			{ ShaderDataType::Float,	"a_TilingFactor"},
			{ ShaderDataType::Int,		"a_EntityID"	}
		});
		s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);

		s_Data->QuadVertexBufferBase = new QuadVertex[s_Data->MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data->MaxIndices);
		s_Data->QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circle

		s_Data->CircleVertexArray = VertexArray::Create();

		s_Data->CircleVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(CircleVertex));
		s_Data->CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Vector3,	"a_WorldPosition"	},
			{ ShaderDataType::Vector3,	"a_LocalPosition"	},
			{ ShaderDataType::Vector4,	"a_Color"			},
			{ ShaderDataType::Float,	"a_Thickness"		},
			{ ShaderDataType::Float,	"a_Fade"			},
			{ ShaderDataType::Int,		"a_EntityID"		}
		});

		s_Data->CircleVertexArray->AddVertexBuffer(s_Data->CircleVertexBuffer);

		s_Data->CircleVertexBufferBase = new CircleVertex[s_Data->MaxVertices];

		s_Data->CircleVertexArray->SetIndexBuffer(quadIB);

		// Lines

		s_Data->LineVertexArray = VertexArray::Create();

		s_Data->LineVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(LineVertex));
		s_Data->LineVertexBuffer->SetLayout({
			{ ShaderDataType::Vector3,	"a_Position" },
			{ ShaderDataType::Vector4,	"a_Color"	 },
			{ ShaderDataType::Int,		"a_EntityID" }
		});

		s_Data->LineVertexArray->AddVertexBuffer(s_Data->LineVertexBuffer);

		s_Data->LineVertexBufferBase = new LineVertex[s_Data->MaxVertices];
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data->MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data->MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data->QuadShader = Shader::Create("Assets/Shaders/Renderer2D_Quad.glsl");
		s_Data->CircleShader = Shader::Create("Assets/Shaders/Renderer2D_Circle.glsl");
		s_Data->LineShader = Shader::Create("Assets/Shaders/Renderer2D_Line.glsl");

		// Set first texture slot to 0
		s_Data->TextureSlots[0] = s_Data->WhiteTexture;

		s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data->CameraUniformBuffer = cameraUniformBuffer;
	}

	void Renderer2D::Reload()
	{
		Shutdown();
		Init(s_Data->CameraUniformBuffer);
	}

	void Renderer2D::Shutdown()
	{
		QS_PROFILE_FUNCTION();

		delete[] s_Data->QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		QS_PROFILE_FUNCTION();

		s_Data->QuadShader->Bind();
		s_Data->QuadShader->SetMatrix4("u_ViewProjection", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& cam)
	{
		QS_PROFILE_FUNCTION();

		QS_CORE_ASSERT(s_Data->RendererSceneState == RendererSceneState::Ended, "Renderer2D: BeginScene()/EndScene() mismatched, did you forget to call EndScene()?");

		s_Data->RendererSceneState = RendererSceneState::Began;

		s_Data->CameraBuffer.ViewProjection = cam.GetViewProjection();
		s_Data->CameraUniformBuffer->SetData(glm::value_ptr(cam.GetViewProjection()), sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const Matrix4& viewProjection)
	{
		QS_PROFILE_FUNCTION();

		QS_CORE_ASSERT(s_Data->RendererSceneState == RendererSceneState::Ended, "Renderer2D: BeginScene()/EndScene() mismatched, did you forget to call EndScene()?");

		s_Data->RendererSceneState = RendererSceneState::Began;

		s_Data->CameraBuffer.ViewProjection = viewProjection;
		s_Data->CameraUniformBuffer->SetData(glm::value_ptr(viewProjection), sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const Camera& camera, const Matrix4& transform)
	{
		QS_PROFILE_FUNCTION();

		QS_CORE_ASSERT(s_Data->RendererSceneState == RendererSceneState::Ended, "Renderer2D: BeginScene()/EndScene() mismatched, did you forget to call EndScene()?");

		s_Data->RendererSceneState = RendererSceneState::Began;

		s_Data->CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data->CameraUniformBuffer->SetData(glm::value_ptr(s_Data->CameraBuffer.ViewProjection), sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		QS_PROFILE_FUNCTION();

		QS_CORE_ASSERT(s_Data->RendererSceneState == RendererSceneState::Began, "Renderer2D: BeginScene()/EndScene() mismatched, did you forget to call BegineScene()?");

		s_Data->RendererSceneState = RendererSceneState::Ended;

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data->QuadIndexCount = 0;
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;

		s_Data->CircleIndexCount = 0;
		s_Data->CircleVertexBufferPtr = s_Data->CircleVertexBufferBase;

		s_Data->LineVertexCount = 0;
		s_Data->LineVertexBufferPtr = s_Data->LineVertexBufferBase;

		s_Data->TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (s_Data->QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
			{
				if (!s_Data->TextureSlots[i])
					continue;

				s_Data->TextureSlots[i]->Bind(i);
				s_Data->Stats.Textures++;
			}

			s_Data->QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);
			s_Data->Stats.DrawCalls++;
		}

		if (s_Data->CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->CircleVertexBufferPtr - (uint8_t*)s_Data->CircleVertexBufferBase);
			s_Data->CircleVertexBuffer->SetData(s_Data->CircleVertexBufferBase, dataSize);

			s_Data->CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data->CircleVertexArray, s_Data->CircleIndexCount);
			s_Data->Stats.DrawCalls++;
		}

		if (s_Data->LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->LineVertexBufferPtr - (uint8_t*)s_Data->LineVertexBufferBase);
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertexBufferBase, dataSize);

			s_Data->LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data->LineWidth);
			RenderCommand::DrawLines(s_Data->LineVertexArray, s_Data->LineVertexCount);
			s_Data->Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawSprite(const Matrix4& transform, const SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else DrawQuad(transform, src.Color, entityID);
	}

	void Renderer2D::DrawCircle(const Matrix4& transform, const Vector4& color, float thickness, float fade, int entityID)
	{
		QS_PROFILE_FUNCTION();

		// TODO: Implement Circles
		//if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		//	NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->CircleVertexBufferPtr->WorldPosition = transform * s_Data->QuadVertexPositions[i];
			s_Data->CircleVertexBufferPtr->LocalPosition = s_Data->QuadVertexPositions[i] * 2.0f;
			s_Data->CircleVertexBufferPtr->Color = color;
			s_Data->CircleVertexBufferPtr->Thickness = thickness;
			s_Data->CircleVertexBufferPtr->Fade = fade;
			s_Data->CircleVertexBufferPtr->EntityID = entityID;
			s_Data->CircleVertexBufferPtr++;
		}

		s_Data->CircleIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const Vector3& p0, const Vector3& p1, const Vector4& color, int entityID)
	{
		s_Data->LineVertexBufferPtr->Position = p0;
		s_Data->LineVertexBufferPtr->Color = color;
		s_Data->LineVertexBufferPtr->EntityID = entityID;
		s_Data->LineVertexBufferPtr++;

		s_Data->LineVertexBufferPtr->Position = p1;
		s_Data->LineVertexBufferPtr->Color = color;
		s_Data->LineVertexBufferPtr->EntityID = entityID;
		s_Data->LineVertexBufferPtr++;

		s_Data->LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const Matrix4& transform, const Vector4& color, int entityID)
	{
		Vector3 lineVerticies[4];
		for (size_t i = 0; i < 4; i++)
			lineVerticies[i] = transform * s_Data->QuadVertexPositions[i];

		DrawLine(lineVerticies[0], lineVerticies[1], color, entityID);
		DrawLine(lineVerticies[1], lineVerticies[2], color, entityID);
		DrawLine(lineVerticies[2], lineVerticies[3], color, entityID);
		DrawLine(lineVerticies[3], lineVerticies[0], color, entityID);
	}

	void Renderer2D::DrawRect(const Vector3& position, const Vector2& size, const Vector4& color, int entityID)
	{
		Vector3 p0 = Vector3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		Vector3 p1 = Vector3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		Vector3 p2 = Vector3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		Vector3 p3 = Vector3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data->LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data->LineWidth = width;
	}

	void Renderer2D::DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const Vector3& position, const Vector2& size, const Vector4& color)
	{
		QS_PROFILE_FUNCTION();

		Matrix4 transform = glm::translate(Matrix4(1.0f), position)
			* glm::scale(Matrix4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const Vector2& position, const Vector2& size, const Ref<Texture2D>& texture, float tilingFactor, const Vector4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const Vector3& position, const Vector2& size, const Ref<Texture2D>& texture, float tilingFactor, const Vector4& tintColor)
	{
		QS_PROFILE_FUNCTION();

		Matrix4 transform = glm::translate(Matrix4(1.0f), position)
			* glm::scale(Matrix4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const Matrix4& transform, const Ref<SubTexture2D>& subTex, float tilingFactor, const Vector4& tintColor, int entityID)
	{
		QS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const Vector2* textureCoords = subTex->GetTextureCoords();

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i] == *subTex->GetTexture())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data->TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = subTex->GetTexture();
			s_Data->TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = tintColor;
			s_Data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data->QuadVertexBufferPtr->EntityID = entityID;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const Matrix4& transform, const Vector4& color, int entityID)
	{
		QS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		const Vector2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data->QuadVertexBufferPtr->EntityID = entityID;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const Matrix4& transform, const Ref<Texture2D>& texture, float tilingFactor, const Vector4& tintColor, int entityID)
	{
		QS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const Vector2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data->TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = tintColor;
			s_Data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data->QuadVertexBufferPtr->EntityID = entityID;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;

		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Vector4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Vector4& color)
	{
		QS_PROFILE_FUNCTION();

		Matrix4 transform = glm::translate(Matrix4(1.0f), position)
			* glm::rotate(Matrix4(1.0f), Math::Radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(Matrix4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const Vector4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const Vector4& tintColor)
	{
		QS_PROFILE_FUNCTION();

		Matrix4 transform = glm::translate(Matrix4(1.0f), position)
			* glm::rotate(Matrix4(1.0f), Math::Radians(rotation), {0.0f, 0.0f, 1.0f})
			* glm::scale(Matrix4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data->Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}
}
