#pragma once

#include "Quelos/Renderer/EditorCamera.h"
#include "Quelos/Core/GUID.h"

#include <entt.hpp>

class b2World;

namespace Quelos
{
	class Entity;
	class ScriptableEntity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithGUID(GUID guid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnSimulationStart();

		void OnRuntimeStop();
		void OnSimulationStop();

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void OnUpdateEditor(TimeStep ts);
		void OnUpdateSimulation(TimeStep ts);
		void OnUpdateRuntime(TimeStep ts);

		void OnRenderEditor(EditorCamera& camera);
		void OnRenderSimulation(EditorCamera& camera);
		void OnRenderRuntime();

		void RenderScene();

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		Entity GetPrimaryCamera();
		Entity GetEntityByGUID(GUID guid);
		
		Entity FindEntityByName(std::string_view name);

		bool IsRunning() const { return m_IsRunning; }

		template <typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename Component>
		void OnComponentAdded(Entity entity, Component& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;

		std::unordered_map<GUID, entt::entity> m_EntityMap;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanal;
	};
}
