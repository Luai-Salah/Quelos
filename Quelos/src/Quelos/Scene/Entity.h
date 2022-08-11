#pragma once

#include "Scene.h"
#include "entt.hpp"

#include "Quelos/Scene/Components.h"

namespace Quelos
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(entt::entity handle, Scene* scene);

		template<typename Component, typename... Args>
		Component& AddComponent(Args&&... args)
		{
			QS_CORE_ASSERT(!HasComponent<Component>(), "Entity already has component!");

			Component& c = m_Scene->m_Registry.emplace<Component>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<Component>(*this, c);
			return c;
		}

		template<typename Component, typename... Args>
		Component& AddOrReplaceComponent(Args&&... args)
		{
			Component& c = m_Scene->m_Registry.emplace_or_replace<Component>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<Component>(*this, c);
			return c;
		}

		template<typename Component>
		Component& GetComponent()
		{
			QS_CORE_ASSERT(HasComponent<Component>(), "Entity doesn't has component!");

			return m_Scene->m_Registry.get<Component>(m_EntityHandle);
		}

		template<typename Component>
		void RemoveComponent()
		{
			QS_CORE_ASSERT(HasComponent<Component>(), "Entity doesn't has component!");

			m_Scene->m_Registry.remove<Component>(m_EntityHandle);
		}

		template<typename Component>
		bool HasComponent()
		{
			return m_Scene->m_Registry.try_get<Component>(m_EntityHandle);
		}

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		GUID GetGUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		operator bool() const { return m_EntityHandle != entt::null; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}
