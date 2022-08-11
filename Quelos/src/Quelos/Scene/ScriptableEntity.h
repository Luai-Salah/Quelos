#pragma once

#include "Entity.h"

namespace Quelos
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename Component>
		Component& GetComponent()
		{
			return m_Entity.GetComponent<Component>();
		}
	protected:
		virtual void OnCreate() {}
		virtual void OnUpdate(TimeStep ts) {}
		virtual void OnDestroy() {}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}
