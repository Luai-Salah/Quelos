#include "QSPCH.h"

#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "Quelos/Core/Application.h"
#include <Quelos/Core/Buffer.h>
#include <Quelos/Core/FileSystem.h>

#include "FileWatch.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/threads.h>

namespace Quelos
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Byte", ScriptFieldType::Byte },

		{ "System.Char", ScriptFieldType::Char },
		{ "System.String", ScriptFieldType::String },

		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },

		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong },

		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },
		
		{ "Quelos.Vector2", ScriptFieldType::Vector2 },
		{ "Quelos.Vector3", ScriptFieldType::Vector3 },
		{ "Quelos.Vector4", ScriptFieldType::Vector4 },

		{ "Quelos.Entity", ScriptFieldType::Entity }
	};

	namespace Utils
	{
		MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPdb = false)
		{
			ScopedBuffer fileData = FileSystem::ReadBytes(assemblyPath);
			if (!fileData)
			{
				QS_CORE_ERROR("Couldn't read data from the assembly {}", assemblyPath);
				return nullptr;
			}

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData.As<char>(), fileData.Size(), 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			if (loadPdb)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					ScopedBuffer pdbFileData = FileSystem::ReadBytes(pdbPath);

					if (pdbFileData)
					{
						mono_debug_open_image_from_memory(image, pdbFileData.As<mono_byte>(), pdbFileData.Size());
						QS_CORE_INFO("Loaded pdb: {}", pdbPath);
					} else QS_CORE_ERROR("Couldn't read data from the pdb {}", pdbPath);
				}
			}

			std::string pathStr = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
			mono_image_close(image);

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				QS_CORE_TRACE("{0}.{1}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			const char* typeName = mono_type_get_name(monoType);

			const auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				QS_CORE_ERROR("Unkown Type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		std::filesystem::path CoreAssemblyFilePath;
		std::filesystem::path AppAssemblyFilePath;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<GUID, Ref<ScriptInstance>> EntityInstances;

		// TODO: Move to Editor Layer
		Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatch;
		bool AssemblyReloadPending = false;

		std::unordered_map<GUID, ScriptFieldMap> EntityScriptFields;

		bool EnableDebugging = true;

		// Runtime
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data;

	static void AppAssemblyFileWatchEvent(const std::string& path, const filewatch::Event changeType)
	{
		if (!s_Data->AssemblyReloadPending && changeType == filewatch::Event::modified)
		{
			s_Data->AssemblyReloadPending = true;

			Application::Get().SubmitToMainThread([]()
			{
				s_Data->AppAssemblyFileWatch.reset();
			ScriptEngine::ReloadAssembly();
			});
		}
	}

	const std::unordered_map<std::string, Ref<ScriptClass>>& ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	Ref<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		if (s_Data->EntityClasses.find(name) == s_Data->EntityClasses.end())
			return nullptr;

		return s_Data->EntityClasses.at(name);
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		QS_CORE_ASSERT(entity, "Entity not found!")

		return s_Data->EntityScriptFields[entity.GetGUID()];
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(Entity entity)
	{
		return GetEntityScriptInstance(entity.GetGUID());
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(GUID entityID)
	{
		auto it = s_Data->EntityInstances.find(entityID);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second;
	}

	MonoObject* ScriptEngine::GetManagedInstance(GUID entityID)
	{
		Ref<ScriptInstance> instance = GetEntityScriptInstance(entityID);
		QS_CORE_ASSERT(instance, "Instance not found!");
		return instance->GetManagedObject();
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* klass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, klass);
		mono_runtime_object_init(instance);

		return instance;
	}

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();

		LoadAssembly("Resources/Scripts/Quelos-ScriptCore.dll");
		bool status = LoadAppAssembly("SandboxProject/Assets/Scripts/Binaries/Sandbox.dll");
		if (!status)
			return;
		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Quelos", "Entity", true);
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& script = entity.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntityClassExists(script.ClassName))
		{
			Ref<ScriptInstance> instance =
				CreateRef<ScriptInstance>(s_Data->EntityClasses[script.ClassName], entity);

			GUID entityID = entity.GetGUID();

			s_Data->EntityInstances[entityID] = instance;

			// Copy fields values
			if (s_Data->EntityScriptFields.find(entityID) != s_Data->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldsMap = s_Data->EntityScriptFields.at(entityID);
				for (const auto& [name, fieldInstance] : fieldsMap)
					instance->SetFieldValueInternal(name, (void*)fieldInstance.m_Buffer);
			}

			instance->InvokeOnCreate();
		}
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	void ScriptEngine::OnStartEntity(Entity entity)
	{
		GUID guid = entity.GetGUID();

		auto it = s_Data->EntityInstances.find(guid);
		if (it == s_Data->EntityInstances.end())
			return;

		it->second->InvokeOnStart();
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, TimeStep ts)
	{
		GUID guid = entity.GetGUID();

		if (s_Data->EntityInstances.find(guid) != s_Data->EntityInstances.end())
			s_Data->EntityInstances[guid]->InvokeOnUpdate(ts);
		else QS_CORE_ERROR("[ScriptEngine::OnUpdateEntity] Couldn't find SciprtInstance for entity {}", guid);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;
		s_Data->EntityInstances.clear();
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullName)
	{
		return s_Data->EntityClasses.find(fullName) != s_Data->EntityClasses.end();
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
	}
	
	void ScriptEngine::LoadAssembly(const std::filesystem::path& path)
	{
		// Create an app domain
		s_Data->AppDomain = mono_domain_create_appdomain((char*)"QuelosScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(path, true);
		QS_CORE_ASSERT(s_Data->CoreAssembly, "[ScriptEngine::LoadAppAssembly] Core Assembly couldn't be loaded!")
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);

		s_Data->CoreAssemblyFilePath = path;
	}
	
	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& path)
	{
		s_Data->AppAssemblyFilePath = path;
		s_Data->AssemblyReloadPending = false;

		s_Data->AppAssembly = Utils::LoadMonoAssembly(path, true);
		if (!s_Data->AppAssembly)
		{
			QS_CORE_ERROR("[ScriptEngine::LoadAppAssembly] App Assembly couldn't be loaded!");
			return false;
		}

		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);

		s_Data->AppAssemblyFileWatch =
			CreateScope<filewatch::FileWatch<std::string>>(path.string(), AppAssemblyFileWatchEvent);

		return true;
	}

	void ScriptEngine::ReloadAssembly()
	{
		QS_PROFILE_FUNCTION();

		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);

		LoadAssembly(s_Data->CoreAssemblyFilePath);
		bool status = LoadAppAssembly(s_Data->AppAssemblyFilePath);
		if (!status)
			return;
		LoadAssemblyClasses();
	
		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Quelos", "Entity", true);
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();

		const MonoTableInfo* typeDefinitionsTable =
			mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Quelos", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullName;

			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else fullName = name;

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity)
				continue;

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, name);
			s_Data->EntityClasses[fullName] = scriptClass;
			int fieldsNum = mono_class_num_fields(monoClass);
			QS_CORE_TRACE("{0}.{1} : {2} Fields", nameSpace, name, fieldsNum);

			void* iterator = nullptr;

			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t fieldFlag = mono_field_get_flags(field);
				if (fieldFlag & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* fieldType = mono_field_get_type(field);
					ScriptFieldType type = Utils::MonoTypeToScriptFieldType(fieldType);

					scriptClass->m_Fields[fieldName] = ScriptField { fieldName, type, field };

					QS_CORE_TRACE("\t-{0} ({1})", fieldName, Utils::ScriptFieldTypeToString(type));
				}

			} 
		}
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		if (s_Data->EnableDebugging)
		{
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=Logs/MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		MonoDomain* monoDomain = mono_jit_init("QuelosJITRuntime");
		QS_CORE_ASSERT(monoDomain, "Couldn't init jit");

		s_Data->RootDomain = monoDomain;

		if (s_Data->EnableDebugging)
			mono_debug_domain_create(s_Data->RootDomain);

		mono_thread_set_main(mono_thread_current());
	}

	MonoObject* ScriptEngine::InvokeMethod(MonoMethod* method, MonoObject* instance, void** params)
	{
		MonoObject* exObj = nullptr;
		MonoObject* result = mono_runtime_invoke(method, instance, params, &exObj);
		if (exObj)
		{
			MonoString* exMonoStr = mono_object_to_string(exObj, nullptr);
			std::string exString = mono_string_to_utf8(exMonoStr);
			QS_CORE_ERROR(exString);
		}

		return result;
	}

	void ScriptEngine::ShutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);

		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(
			isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage,
			classNamespace.c_str(),
			className.c_str()
		);
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), paramCount);
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnStartMethod = scriptClass->GetMethod("OnStart", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		{
			GUID entityID = entity.GetGUID();
			void* param = &entityID;
			ScriptEngine::InvokeMethod(m_Constructor, m_Instance, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (!m_OnCreateMethod)
			return;

		ScriptEngine::InvokeMethod(m_OnCreateMethod, m_Instance);
	}

	void ScriptInstance::InvokeOnStart()
	{
		if (!m_OnStartMethod)
			return;

		ScriptEngine::InvokeMethod(m_OnStartMethod, m_Instance);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (!m_OnUpdateMethod)
			return;

		void* param = &ts;
		ScriptEngine::InvokeMethod(m_OnUpdateMethod, m_Instance, &param);
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		mono_field_get_value(m_Instance, it->second.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, void* data)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		mono_field_set_value(m_Instance, it->second.ClassField, data);
		return true;
	}
}
