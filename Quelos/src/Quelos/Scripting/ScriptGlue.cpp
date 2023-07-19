#include "QSPCH.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Quelos/Core/Input.h"
#include "Quelos/Physics/Physics2D.h"

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

		QS_CORE_INFO(msg);

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

	static uint64_t Entity_FindEntityByName(MonoString* nameMonoString)
	{
		char* entityNameCStr = mono_string_to_utf8(nameMonoString);

		Scene* scene = ScriptEngine::GetSceneContext();
		QS_CORE_ASSERT(scene, "Scene is null!");

		Entity entity = scene->FindEntityByName(entityNameCStr);
		mono_free(entityNameCStr);

		if (!entity)
			return 0;

		return entity.GetGUID();
	}

	static MonoObject* Entity_GetScriptInstance(GUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static void Transform_GetPosition(GUID guid, glm::vec3* outPosition)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		*outPosition = entity.GetComponent<TransformComponent>().Position;
	}

	static void Transform_SetPosition(GUID guid, glm::vec3* position)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		entity.GetComponent<TransformComponent>().Position = *position;
	}
	
	static Rigidbody2DComponent::BodyType Rigidbody2D_GetBodyType(GUID guid)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		return rb2d.Type;
	}

	static void Rigidbody2D_SetBodyType(GUID guid, Rigidbody2DComponent::BodyType type)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = rb2d.RuntimeBody;
		rb2d.Type = type;
		body->SetType(Utils::Rigidbody2DTypeToBox2D(type));
	}

	static void Rigidbody2D_GetLinearVelocity(GUID guid, glm::vec2* outVelocity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = rb2d.RuntimeBody;
		const b2Vec2& velocity = body->GetLinearVelocity();

		*outVelocity = glm::vec2(velocity.x, velocity.y);
	}

	static void Rigidbody2D_SetLinearVelocity(GUID guid, glm::vec2* velocity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();

		Entity entity = scene->GetEntityByGUID(guid);
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = rb2d.RuntimeBody;
		body->SetLinearVelocity(b2Vec2(velocity->x, velocity->y));
	}
	
	static void Rigidbody2D_AddLinearImpulseToCenter(GUID guid, glm::vec2* impulse, bool wake)
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
			size_t pos = typeName.find_last_of(':') + 1;
			size_t count = typeName.length() - 9 - pos;
			std::string_view componentName = typeName.substr(pos, count);

			std::string managedTypeName = fmt::format("Quelos.{}", componentName);

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
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{ });
	}

	void ScriptGlue::RegisterFunctions()
	{
		QS_ADD_INTERNAL_CALL(NativeLog);

		QS_ADD_INTERNAL_CALL(Entity_HasComponent);
		QS_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		QS_ADD_INTERNAL_CALL(Entity_GetScriptInstance);

		QS_ADD_INTERNAL_CALL(Transform_GetPosition);
		QS_ADD_INTERNAL_CALL(Transform_SetPosition);

		QS_ADD_INTERNAL_CALL(Rigidbody2D_GetBodyType);
		QS_ADD_INTERNAL_CALL(Rigidbody2D_SetBodyType);
		QS_ADD_INTERNAL_CALL(Rigidbody2D_GetLinearVelocity);
		QS_ADD_INTERNAL_CALL(Rigidbody2D_SetLinearVelocity);
		QS_ADD_INTERNAL_CALL(Rigidbody2D_AddLinearImpulseToCenter);

		QS_ADD_INTERNAL_CALL(Input_GetKey);
	}
}
