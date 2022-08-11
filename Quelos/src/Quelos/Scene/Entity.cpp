#include "QSPCH.h"
#include "Entity.h"

#include "ScriptableEntity.h"

namespace Quelos
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}
