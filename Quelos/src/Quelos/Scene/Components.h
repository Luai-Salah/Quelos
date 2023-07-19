#pragma once

#include "QSPCH.h"
#include "Quelos/Scene/SceneCamera.h"
#include "Quelos/Renderer/Texture.h"
#include "Quelos/Renderer/SubTexture2D.h"

#include "Quelos/Core/GUID.h"

#include <entt.hpp>
#include <box2d/b2_body.h>

namespace Quelos
{
	struct IDComponent
	{
		GUID ID;

		IDComponent(const GUID& guid) : ID(guid) {}
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) :Tag(tag) { }
	};

	struct TransformComponent
	{
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position, glm::vec3 size = glm::vec3{ 1.0f }, float rotation = 0.0f)
			: Position(position), Scale(size), Rotation(rotation) { }

		glm::mat4 GetTransformMatrix() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

			return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f };
		Ref<SubTexture2D> Texture;
		glm::vec2 MinTextureCoords{ 0.0f };
		glm::vec2 MaxTextureCoords{ 1.0f };
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) { }
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: Think about moving to "Scene".
		bool FixedAspectRatio = false;
		entt::entity TargetID = entt::null;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	// Forword Decloration
	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		template<typename T>
		void Bind()
		{
			Instance = static_cast<ScriptableEntity*>(new T());
		}
	};

	// Physics

	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic = 1, Kinematic = 2 };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		b2Body* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;

		void ApplyForceToCenter(glm::vec2 force)
		{
			RuntimeBody->ApplyForceToCenter({ force.x, force.y }, true);
		}
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f , 0.0f };
		glm::vec2 Size = { 0.5f , 0.5f };

		// TODO: Move into physics material
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f , 0.0f };
		float Radius = 0.5f;

		// TODO: Move into physics material
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	template<typename... Component>
	struct ComponentGroup { };

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		CircleRendererComponent, CircleCollider2DComponent, NativeScriptComponent,
		Rigidbody2DComponent, BoxCollider2DComponent, CameraComponent, ScriptComponent>;
}
