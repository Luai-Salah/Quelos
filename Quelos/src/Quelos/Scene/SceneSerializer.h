#pragma once

#include "Scene.h"
#include "Quelos/Core/Core.h"

namespace Quelos
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::filesystem::path& filePath);
		void SerializeRuntime(const std::string& filePath);

		bool Deserialize(const std::filesystem::path& filePath);
		bool DeserializeRuntime(const std::string& filePath);
	private:
		Ref<Scene> m_Scene;
	};
}
