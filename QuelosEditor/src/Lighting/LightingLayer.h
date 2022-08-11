#pragma once

#include <Quelos.h>

using namespace Quelos;

namespace Lighting
{
	class Lighting
	{
	public:
		Lighting(EditorCamera& eCam);
		~Lighting();

		void OnUpdate(TimeStep ts);
		void OnImGuiRender();
	private:
		EditorCamera& m_EditorCamera;
		Matrix4 m_Transform;

		struct Material
		{
			Vector4 Ambient{ 1.0f };
			float Shininess = 32.0f;
		};

		struct Light
		{
			Vector4 Position;

			Vector4 Ambient;
			Vector4 Diffuse;
			Vector4 Specular;
		};

		Light m_Light;
		Material m_Material;
		Ref<Texture2D> m_Container;
		Ref<Texture2D> m_ContainerSpecular;

		Ref<UniformBuffer> m_MaterialUB;
		Ref<UniformBuffer> m_LightUB;
		Ref<UniformBuffer> m_ViewPosition;

		Ref<Shader> m_CubeShader;
		Ref<VertexArray> m_CubeVA;

		Ref<Shader> m_LightingShader;
		Ref<VertexArray> m_LightingVA;

		Ref<Shader> m_LampShader;
		Ref<VertexArray> m_LampVA;

		Vector3 m_LightPosition{ -0.2f, 1.0f, -0.3f };
	};
}
