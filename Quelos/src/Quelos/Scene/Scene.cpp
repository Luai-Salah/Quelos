#include "QSPCH.h"
#include "Scene.h"

#include "Components.h"
#include "Quelos/Renderer/Renderer2D.h"
#include "Quelos/Renderer/Renderer.h"
#include "Quelos/Renderer/RenderCommand.h"
#include "Quelos/Scene/ScriptableEntity.h"
#include "Quelos/Scripting/ScriptEngine.h"

#include "Quelos/Physics/Physics2D.h"
#include "Entity.h"

namespace Quelos
{
	Scene::Scene()
	{
	}

	void Scene::OnUpdateEditor(TimeStep ts)
	{
	}

	void Scene::OnUpdateSimulation(TimeStep ts)
	{
		// Physics Simulation
		if (m_PhysicsWorld)
		{
			const int32_t velocityIteration = 6;
			const int32_t positionIteration = 2;

			m_PhysicsWorld->Step(ts, velocityIteration, positionIteration);

			m_Registry.view<Rigidbody2DComponent>().each([&](auto e, Rigidbody2DComponent& rb)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();

				b2Body* body = rb.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.Rotation.z = body->GetAngle();
			});
		}
	}

	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		// On Update
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc)
			{
				nsc.Instance->OnUpdate(ts);
			});
		}

		{
			// Update all script entities
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnUpdateEntity(entity, ts);
			}
		}

		// Physics
		{
			const int32_t velocityIteration = 6;
			const int32_t positionIteration = 2;

			m_PhysicsWorld->Step(ts, velocityIteration, positionIteration);

			m_Registry.view<Rigidbody2DComponent>().each([&](auto e, Rigidbody2DComponent& rb)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();

				b2Body* body = rb.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.Rotation.z = body->GetAngle();
			});
		}
	}

	void Scene::OnRenderEditor(EditorCamera& camera)
	{
		// Renderering
		Renderer2D::BeginScene(camera);

		RenderScene();

		Renderer2D::EndScene();
	}

	void Scene::OnRenderSimulation(EditorCamera& camera)
	{
		// Renderering
		Renderer2D::BeginScene(camera);

		RenderScene();

		Renderer2D::EndScene();
	}

	void Scene::OnRenderRuntime()
	{
		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 camTransform;
		{
			auto group = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);

			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					if (camera.TargetID != entt::null)
						transform.Position = m_Registry.get<TransformComponent>(camera.TargetID).Position;

					mainCamera = &camera.Camera;
					camTransform = transform.GetTransformMatrix();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, camTransform);

			RenderScene();

			Renderer2D::EndScene();
		}
	}

	void Scene::RenderScene()
	{
		// Draw sprites
		{
			auto group = m_Registry.group<TransformComponent, SpriteRendererComponent>();
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransformMatrix(), sprite, (int)entity);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, c] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransformMatrix(), c.Color, c.Thickness, c.Fade, (int)entity);
			}
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRation cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camera = view.get<CameraComponent>(entity);
			if (!camera.FixedAspectRatio)
			{
				camera.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::GetPrimaryCamera()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	Entity Scene::GetEntityByGUID(GUID guid)
	{
		if (m_EntityMap.find(guid) != m_EntityMap.end())
			return { m_EntityMap.at(guid), this };

		return { };
	}

	Entity Scene::FindEntityByName(std::string_view name)
	{
		for (auto& entity : m_Registry.view<TagComponent>())
		{
			const auto& tagComponent = m_Registry.get<TagComponent>(entity);
			if (tagComponent.Tag == name)
				return { entity, this };
		}

		return { };
	}

	Scene::~Scene()
	{
	}

	template <typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<GUID, entt::entity>& enttMap)
	{
		([&]()
		{
			src.view<Component>().each([&](entt::entity e, Component& c)
			{
				GUID guid = src.get<IDComponent>(e).ID;
				QS_CORE_ASSERT(enttMap.find(guid) != enttMap.end(), "Error: GUID not found in the entities map!");
				entt::entity dstEID = enttMap.at(guid);

				dst.emplace_or_replace<Component>(dstEID, c);
			});
		}(), ...);
	}

	template <typename... Component>
	static void CopyComponent
	(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src,const std::unordered_map<GUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template <typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template <typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_EntityMap = other->m_EntityMap;

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegs = other->m_Registry;
		auto& destSceneRegs = newScene->m_Registry;
		std::unordered_map<GUID, entt::entity> enttMap;

		auto idView = srcSceneRegs.view<IDComponent>();
		const entt::entity* data = idView.data();
		auto size = idView.size();

		for (int i = 0; i < size; i++)
		{
			GUID guid = srcSceneRegs.get<IDComponent>(data[i]).ID;
			const auto& name = srcSceneRegs.get<TagComponent>(data[i]).Tag;
			std::string scriptName;
			if (srcSceneRegs.has<ScriptComponent>(data[i]))
				scriptName = srcSceneRegs.get<ScriptComponent>(data[i]).ClassName;
			Entity newEntity = newScene->CreateEntityWithGUID(guid, name, scriptName);
			enttMap[guid] = (entt::entity)newEntity;
		}

		// Copy components
		CopyComponent(AllComponents{ }, destSceneRegs, srcSceneRegs, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name) { return CreateEntityWithGUID(GUID(), name); }
	Entity Scene::CreateEntityWithGUID(GUID guid, const std::string& name, const std::string& scriptName)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(guid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		if (!scriptName.empty())
		{
			if (ScriptEngine::EntityClassExists(scriptName))
				ScriptEngine::OnCreateEntity(entity, scriptName);
		}

		m_EntityMap[guid] = entity;

		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName() + "_");

		// Copy components
		CopyComponentIfExists(AllComponents{ }, newEntity, entity);

		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetGUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;
		OnPhysics2DStart();

		ScriptEngine::OnRuntimeStart(this);

		{
			// Instantiate all script entities
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnStartEntity(entity);
			}
		}
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;
		OnPhysics2DStop();

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Utils::Rigidbody2DTypeToBox2D(rb.Type);
			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb.FixedRotation);

			rb.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y,
					b2Vec2(bc2d.Offset.x, bc2d.Offset.x), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				bc2d.RuntimeFixture = body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius * transform.Scale.x;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	template<typename Component>
	void Scene::OnComponentAdded(Entity entity, Component& component)
	{
		static_assert(sizeof(Component) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& transform)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& transform)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& camera)
	{
		camera.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}
}
