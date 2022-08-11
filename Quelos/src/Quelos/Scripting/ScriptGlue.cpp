#include "QSPCH.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Quelos/Core/Input.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <Quelos/Scene/Scene.h>

namespace Quelos
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define QS_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Quelos.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* text)
	{
		char* cStr = mono_string_to_utf8(text);
		std::string msg(cStr);

		QS_CORE_LOG(msg);

		mono_free(cStr);
	}

	static bool Entity_HasComponent(GUID guid, MonoReflectionType* componentType)
	{
		MonoType* managedType = mono_reflection_type_get_type(componentType);

		Scene* scene = ScriptEngine::GetSceneContext();
		QS_CORE_ASSERT(scene, "Scene is null!");

		Entity entity = scene->GetEntityByGUID(guid);
		QS_CORE_ASSERT(entity, "Entity is null");

		QS_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(),
			"Managed type wasn't found!");

		return s_EntityHasComponentFuncs[managedType](entity);
	}

	static void TransformComponent_GetPosition(GUID guid, Vector3* outPosition)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		*outPosition = entity.GetComponent<TransformComponent>().Position;
	}

	static void TransformComponent_SetPosition(GUID guid, Vector3* position)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		entity.GetComponent<TransformComponent>().Position = *position;
	}

	static void Rigidbody2DComponent_AddLinearImpulseToCenter(GUID guid, Vector2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_GetKey(KeyCode keyCode)
	{
		return Input::GetKey(keyCode);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([&]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);

			std::string managedTypeName = fmt::format("Quelos.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(),
				ScriptEngine::GetCoreAssemblyImage());

			if (!managedType)
			{
				QS_CORE_ERROR("Could not find component type {}", managedTypeName);
				return;
			}

			s_EntityHasComponentFuncs[managedType] =
				[](Entity entity) { return entity.HasComponent<Component>(); };
		}(), ...);
	}

	template<typename... Components>
	static void RegisterComponent(ComponentGroup<Components...>)
	{
		RegisterComponent<Components...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent(AllComponents{ });
	}

	void ScriptGlue::RegisterFunctions()
	{
		QS_ADD_INTERNAL_CALL(NativeLog);

		QS_ADD_INTERNAL_CALL(Entity_HasComponent);

		QS_ADD_INTERNAL_CALL(TransformComponent_GetPosition);
		QS_ADD_INTERNAL_CALL(TransformComponent_SetPosition);

		QS_ADD_INTERNAL_CALL(Rigidbody2DComponent_AddLinearImpulseToCenter);

		QS_ADD_INTERNAL_CALL(Input_GetKey);
	}
}
