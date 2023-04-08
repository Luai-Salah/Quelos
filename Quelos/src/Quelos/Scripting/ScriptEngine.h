#pragma once

#include <string>
#include <Quelos/Scene/Scene.h>
#include <Quelos/Scene/Entity.h>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoClassField MonoClassField;
}

namespace Quelos
{
	enum class ScriptFieldType
	{
		None = 0,
		Bool, Byte,
		Char, String,
		Short, Int, Long,
		UShort, UInt, ULong,
		Float, Double,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		std::string Name;
		ScriptFieldType Type = ScriptFieldType::None;
		MonoClassField* ClassField = nullptr;
	};


	class ScriptFieldInstance
	{
	public:
		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue() const
		{
			static_assert(sizeof(T) <= 16, "sizeof(T) expected <= 16 but found greater size");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			const size_t size = sizeof(T);
			static_assert(size <= 16, "sizeof(T) expected <= 16 but found greater size");
			memcpy(m_Buffer, (const void*)&value, size);
		}
	public:
		ScriptField Field;
	private:
		uint8_t m_Buffer[16];

		friend class ScriptEngine;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* Instantiate();

		MonoMethod* GetMethod(const std::string& name, int paramCount = 0);
		const std::unordered_map<std::string, ScriptField>& GetFields() { return m_Fields; }
	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		std::unordered_map<std::string, ScriptField> m_Fields;

		MonoClass* m_MonoClass = nullptr;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnStart();
		void InvokeOnUpdate(float ts);

		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "sizeof(T) expected <= 16 but was greater");

			return GetFieldValueInternal(name, s_FieldValueBuffer) ? *(T*)s_FieldValueBuffer : T();
		}

		template<typename T>
		bool SetFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= 16, "sizeof(T) expected <= 16 but was greater");

			return SetFieldValueInternal(name, (void*)&value);
		}

		MonoObject* GetManagedObject() const { return m_Instance; }
	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, void* value);
	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;

		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnStartMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		inline static uint8_t s_FieldValueBuffer[16];

		friend class ScriptEngine;
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnCreateEntity(Entity entity);
		static void OnStartEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, TimeStep ts);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static bool EntityClassExists(const std::string& fullName);
		
		static void LoadAssembly(const std::filesystem::path& path);
		static bool LoadAppAssembly(const std::filesystem::path& path);

		static void ReloadAssembly();

		static MonoObject* InvokeMethod(MonoMethod* method, MonoObject* instance, void** params = nullptr);

		static const std::unordered_map<std::string, Ref<ScriptClass>>& GetEntityClasses();
		static Ref<ScriptClass> GetEntityClass(const std::string& entity);
		static ScriptFieldMap& GetScriptFieldMap(Entity entityID);

		static MonoImage* GetCoreAssemblyImage();
		static Scene* GetSceneContext();
		
		static Ref<ScriptInstance> GetEntityScriptInstance(Entity entity);
		static Ref<ScriptInstance> GetEntityScriptInstance(GUID entityID);

		static MonoObject* GetManagedInstance(GUID entityID);
	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses();

		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class ScriptClass;
		friend class ScriptGlue;
	};

	namespace Utils
	{
		inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
		{
			switch (fieldType)
			{
				case ScriptFieldType::None:			return "None";

				case ScriptFieldType::Bool:			return "Bool";
				case ScriptFieldType::Byte:			return "Byte";

				case ScriptFieldType::Char:			return "Char";
				case ScriptFieldType::String:		return "String";

				case ScriptFieldType::Short:		return "Short";
				case ScriptFieldType::Int:			return "Int";
				case ScriptFieldType::Long:			return "Long";

				case ScriptFieldType::UShort:		return "UShort";
				case ScriptFieldType::UInt:			return "UInt";
				case ScriptFieldType::ULong:		return "ULong";

				case ScriptFieldType::Float:		return "Float";
				case ScriptFieldType::Double:		return "Double";

				case ScriptFieldType::Vector2:		return "Vector2";
				case ScriptFieldType::Vector3:		return "Vector3";
				case ScriptFieldType::Vector4:		return "Vector4";

				case ScriptFieldType::Entity:		return "Entity";
			}
			QS_CORE_ASSERT(false, "Unknown ScriptFieldType")

			return "Invalid";
		}
		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view stringFieldType)
		{
			if		(stringFieldType == "None")			return ScriptFieldType::None;
			else if (stringFieldType == "Bool")			return ScriptFieldType::Bool;
			else if (stringFieldType == "Byte")			return ScriptFieldType::Byte;
			else if (stringFieldType == "Char")			return ScriptFieldType::Char;
			else if (stringFieldType == "String")		return ScriptFieldType::String;
			else if (stringFieldType == "Short")		return ScriptFieldType::Short;
			else if (stringFieldType == "Int")			return ScriptFieldType::Int;
			else if (stringFieldType == "Long")			return ScriptFieldType::Long;
			else if (stringFieldType == "Float")		return ScriptFieldType::Float;
			else if (stringFieldType == "Double")		return ScriptFieldType::Double;
			else if (stringFieldType == "Vector2")		return ScriptFieldType::Vector2;
			else if (stringFieldType == "Vector3")		return ScriptFieldType::Vector3;
			else if (stringFieldType == "Vector4")		return ScriptFieldType::Vector4;

			QS_CORE_ASSERT(false, "Unknown ScriptFieldType")

			return ScriptFieldType::None;
		}
	}
}
