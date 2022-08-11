#include "QSPCH.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<Vector3>
	{
		static Node encode(const Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Vector2>
	{
		static Node encode(const Vector2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, Vector2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Vector4>
	{
		static Node encode(const Vector4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, Vector4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Quelos
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string Rigidbody2DTypeToString(Rigidbody2DComponent::BodyType type)
	{
		switch (type)
		{
			case Rigidbody2DComponent::BodyType::Static:	return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic:	return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		QS_CORE_ASSERT(false, "Unknow Body Type");
		return "";
	}

	static Rigidbody2DComponent::BodyType Rigidbody2DTypeFromString(std::string typeString)
	{
		if (typeString == "Static")
			return Rigidbody2DComponent::BodyType::Static;
		else if (typeString == "Dynamic")
			return Rigidbody2DComponent::BodyType::Dynamic;
		else if (typeString == "Kinematic")
			return Rigidbody2DComponent::BodyType::Kinematic;

		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		QS_CORE_ASSERT(entity.HasComponent<IDComponent>(), "No ID Compoenent");

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "EntityID" << YAML::Value << entity.GetGUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transform.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto cameraComponent = entity.GetComponent<CameraComponent>();
			auto camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera

			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();

			out << YAML::Key << "PresVerticalFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PresNearClip" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PresFarClip" << YAML::Value << camera.GetPerspectiveFarClip();

			out << YAML::Key << "OrthoSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthoNearClip" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthoFarClip" << YAML::Value << camera.GetOrthographicFarClip();

			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "TargetID" << YAML::Value << (uint32_t)cameraComponent.TargetID;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto scriptComponent = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "Name" << YAML::Value << scriptComponent.ClassName;

			out << YAML::EndMap; // ScriptComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRenderer.Color;
			if (spriteRenderer.Texture != nullptr)
				out << YAML::Key << "TexturePath" << YAML::Value << spriteRenderer.Texture->GetTexture()->GetPath().string();
			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRenderer.TilingFactor;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			auto cRenderer = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << cRenderer.Color;
			out << YAML::Key << "Thickness" << YAML::Value << cRenderer.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << cRenderer.Fade;

			out << YAML::EndMap; // CircleRendererComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto rb = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << Rigidbody2DTypeToString(rb.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb.FixedRotation;

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto boxCollider2d = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider2d.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider2d.Size;
			out << YAML::Key << "Density" << YAML::Value << boxCollider2d.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxCollider2d.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << boxCollider2d.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << boxCollider2d.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto circleCollider2d = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << circleCollider2d.Offset;
			out << YAML::Key << "Radius" << YAML::Value << circleCollider2d.Radius;
			out << YAML::Key << "Density" << YAML::Value << circleCollider2d.Density;
			out << YAML::Key << "Friction" << YAML::Value << circleCollider2d.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << circleCollider2d.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << circleCollider2d.RestitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		out << YAML::EndMap; // Entity
	}
	
	void SceneSerializer::Serialize(const std::filesystem::path& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filePath.stem().string();
		out << YAML::Key << "Entites" << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](entt::entity entityID)
		{
			Entity entity = { entityID, m_Scene.get() };

			if (!entity)
				return;

			SerializeEntity(out, entity);
		});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}
	
	void SceneSerializer::SerializeRuntime(const std::string& filePath)
	{
		// Not implemented
		QS_CORE_ASSERT(false, "");
	}
	
	bool SceneSerializer::Deserialize(const std::filesystem::path & filePath)
	{
		YAML::Node data;

		try
		{
			data = YAML::LoadFile(filePath.string());
		}
		catch (const YAML::ParserException& ex)
		{
			QS_CORE_ERROR("Failed to load .quelos from path: '{0}'. (YAML Exeption: {1})", filePath.string(), ex.what());
			return false;
		}

		if (!data["Scene"])
		{
			QS_CORE_ERROR("Couldn't find Node 'Scene' in file [Path: '{0}']", filePath.string());
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		QS_CORE_TRACE("Deserializing Scene '{0}'", sceneName);

		auto entites = data["Entites"];

		if (entites)
		{
			for (auto entity : entites)
			{
				uint64_t guid = entity["EntityID"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->CreateEntityWithGUID(guid, name);

				auto transform = entity["TransformComponent"];
				if (transform)
				{
					// Entities always have a transformComponent
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Position = transform["Position"].as<Vector3>();
					tc.Rotation = transform["Rotation"].as<Vector3>();
					tc.Scale = transform["Scale"].as<Vector3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PresVerticalFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PresNearClip"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PresFarClip"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthoSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthoNearClip"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthoFarClip"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					if (cameraComponent["TargetID"])
						cc.TargetID = (entt::entity)cameraComponent["TargetID"].as<uint32_t>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();

					if (scriptComponent["Name"].IsDefined())
						sc.ClassName = scriptComponent["Name"].as<std::string>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& sr = deserializedEntity.AddComponent<SpriteRendererComponent>();

					if (spriteRendererComponent["Color"].IsDefined())
						sr.Color = spriteRendererComponent["Color"].as<Vector4>();

					if (spriteRendererComponent["TexturePath"].IsDefined())
					{
						std::string texPath = spriteRendererComponent["TexturePath"].as<std::string>();
						if (!texPath.empty())
							sr.Texture = CreateRef<SubTexture2D>(AssetsManager::GetTexture(texPath));
					}

					if (spriteRendererComponent["TilingFactor"].IsDefined())
						sr.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

				auto cRc = entity["CircleRendererComponent"];
				if (cRc)
				{
					auto& cr = deserializedEntity.AddComponent<CircleRendererComponent>();

					auto color = cRc["Color"];
					if (color)
						cr.Color = color.as<Vector4>();

					auto thickness = cRc["Thickness"];
					if (thickness)
						cr.Thickness = thickness.as<float>();

					auto fade = cRc["Fade"];
					if (fade)
						cr.Fade = fade.as<float>();
				}

				auto rb2Dc = entity["Rigidbody2DComponent"];
				if (rb2Dc)
				{
					auto& rb = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb.Type = Rigidbody2DTypeFromString(rb2Dc["BodyType"].as<std::string>());
					rb.FixedRotation = rb2Dc["FixedRotation"].as<bool>();
				}

				auto bc2dc = entity["BoxCollider2DComponent"];
				if (bc2dc)
				{
					auto& bc = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc.Offset = bc2dc["Offset"].as<Vector2>();
					bc.Size = bc2dc["Size"].as<Vector2>();

					bc.Density = bc2dc["Density"].as<float>();
					bc.Friction = bc2dc["Friction"].as<float>();
					bc.Restitution = bc2dc["Restitution"].as<float>();
					bc.RestitutionThreshold = bc2dc["RestitutionThreshold"].as<float>();
				}

				auto cc2dc = entity["CircleCollider2DComponent"];
				if (cc2dc)
				{
					auto& cc = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc.Offset = cc2dc["Offset"].as<Vector2>();
					cc.Radius = cc2dc["Radius"].as<float>();

					cc.Density = cc2dc["Density"].as<float>();
					cc.Friction = cc2dc["Friction"].as<float>();
					cc.Restitution = cc2dc["Restitution"].as<float>();
					cc.RestitutionThreshold = cc2dc["RestitutionThreshold"].as<float>();
				}
			}
		}

		return true;
	}
	
	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		// Not implemented
		QS_CORE_ASSERT(false, "");
		return false;
	}
}
