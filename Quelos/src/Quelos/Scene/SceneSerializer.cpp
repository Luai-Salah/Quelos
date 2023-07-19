#include "QSPCH.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>
#include <Quelos/Scripting/ScriptEngine.h>

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
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
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
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

	template<>
	struct convert<Quelos::GUID>
	{
		static Node encode(const Quelos::GUID& guid)
		{
			Node node;
			node.push_back((uint64_t)guid);
			return node;
		}

		static bool decode(const Node& node, Quelos::GUID& guid)
		{
			guid = node.as<uint64_t>();
			return true;
		}
	};
}

namespace Quelos
{
#define WRITE_SCRIPT_FIELD(FieldType, Type)						\
			case ScriptFieldType::FieldType:			\
				out << scriptField.GetValue<Type>();	\
				break

#define READ_SCRIPT_FIELD(FieldType, Type)						\
			case Quelos::ScriptFieldType::FieldType:			\
			{													\
				Type data = scriptField["Data"].as<Type>();		\
				instance.SetValue(data);						\
				break;											\
			}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
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
			out << YAML::BeginMap; // ScriptComponent

			auto scriptComponent = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "Name" << YAML::Value << scriptComponent.ClassName;

			Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
			if (entityClass)
			{
				const auto& fields = entityClass->GetFields();

				if (fields.size() < 0)
				{
					out << YAML::EndMap; // ScriptComponent
					return;
				}

				out << YAML::Key << "ScriptFields" << YAML::Value;
				{
					out << YAML::BeginSeq; // ScriptFields

					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					for (const auto& [name, field] : fields)
					{
						if (entityFields.find(name) == entityFields.end())
							continue;

						out << YAML::BeginMap; // FieldData
						{
							out << YAML::Key << "Name" << YAML::Value << name;
							out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

							ScriptFieldInstance& scriptField = entityFields.at(name);

							out << YAML::Key << "Data" << YAML::Value;

							switch (field.Type)
							{
								WRITE_SCRIPT_FIELD(None, uint64_t);
								WRITE_SCRIPT_FIELD(Bool, bool);
								WRITE_SCRIPT_FIELD(Byte, uint8_t);
								WRITE_SCRIPT_FIELD(Char, char);
								WRITE_SCRIPT_FIELD(String, uint64_t);
								WRITE_SCRIPT_FIELD(Short, int16_t);
								WRITE_SCRIPT_FIELD(Int, int32_t);
								WRITE_SCRIPT_FIELD(Long, int64_t);
								WRITE_SCRIPT_FIELD(UShort, uint16_t);
								WRITE_SCRIPT_FIELD(UInt, uint32_t);
								WRITE_SCRIPT_FIELD(ULong, uint64_t);
								WRITE_SCRIPT_FIELD(Float, float);
								WRITE_SCRIPT_FIELD(Double, double);
								WRITE_SCRIPT_FIELD(Float2, glm::vec2);
								WRITE_SCRIPT_FIELD(Float3, glm::vec3);
								WRITE_SCRIPT_FIELD(Float4, glm::vec4);
								WRITE_SCRIPT_FIELD(Entity, GUID);
							}
						}
						out << YAML::EndMap; // FieldData
					}

					out << YAML::EndSeq; // ScriptFields
				}

				out << YAML::EndMap; // ScriptComponent
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			const auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRenderer.Color;
			if (spriteRenderer.Texture != nullptr)
			{
				std::string path = AssetsManager::GetRelativePath(spriteRenderer.Texture->GetTexture()->GetPath()).string();
				out << YAML::Key << "TexturePath" << YAML::Value << path;
			}
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
					tc.Position = transform["Position"].as<glm::vec3>();
					tc.Rotation = transform["Rotation"].as<glm::vec3>();
					tc.Scale = transform["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto cameraProps = cameraComponent["Camera"];
					cc.Camera.SetViewportSize(1, 1);
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

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<ScriptClass> scriptClass = ScriptEngine::GetEntityClass(sc.ClassName);
						if (scriptClass)
						{
							const auto& fields = scriptClass->GetFields();

							auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

							for (auto scriptField : scriptFields)
							{
								std::string name = scriptField["Name"].as<std::string>();
								std::string typeString = scriptField["Type"].as<std::string>();
								ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

								ScriptFieldInstance& instance = entityFields[name];
								if (fields.find(name) == fields.end())
								{
									QS_ERROR("Field {} not found.", name);
									continue;
								}

								instance.Field = fields.at(name);

								switch (type)
								{
									READ_SCRIPT_FIELD(None, uint64_t);
									READ_SCRIPT_FIELD(Bool, bool);
									READ_SCRIPT_FIELD(Byte, uint8_t);
									READ_SCRIPT_FIELD(Char, char);
									READ_SCRIPT_FIELD(String, uint64_t);
									READ_SCRIPT_FIELD(Short, int16_t);
									READ_SCRIPT_FIELD(Int, int32_t);
									READ_SCRIPT_FIELD(Long, int64_t);
									READ_SCRIPT_FIELD(UShort, uint16_t);
									READ_SCRIPT_FIELD(UInt, uint32_t);
									READ_SCRIPT_FIELD(ULong, uint64_t);
									READ_SCRIPT_FIELD(Float, float);
									READ_SCRIPT_FIELD(Double, double);
									READ_SCRIPT_FIELD(Float2, glm::vec2);
									READ_SCRIPT_FIELD(Float3, glm::vec3);
									READ_SCRIPT_FIELD(Float4, glm::vec4);
									READ_SCRIPT_FIELD(Entity, GUID);
								}
							}
						}
					}
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& sr = deserializedEntity.AddComponent<SpriteRendererComponent>();

					if (spriteRendererComponent["Color"].IsDefined())
						sr.Color = spriteRendererComponent["Color"].as<glm::vec4>();

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
						cr.Color = color.as<glm::vec4>();

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
					bc.Offset = bc2dc["Offset"].as<glm::vec2>();
					bc.Size = bc2dc["Size"].as<glm::vec2>();

					bc.Density = bc2dc["Density"].as<float>();
					bc.Friction = bc2dc["Friction"].as<float>();
					bc.Restitution = bc2dc["Restitution"].as<float>();
					bc.RestitutionThreshold = bc2dc["RestitutionThreshold"].as<float>();
				}

				auto cc2dc = entity["CircleCollider2DComponent"];
				if (cc2dc)
				{
					auto& cc = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc.Offset = cc2dc["Offset"].as<glm::vec2>();
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
