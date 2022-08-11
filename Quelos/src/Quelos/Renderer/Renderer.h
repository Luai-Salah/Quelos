#pragma once

#include "RendererAPI.h"
#include "OrthographicCamera.h"
#include "Shader.h"

namespace Quelos
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResized(uint32_t width, uint32_t heigth);

		static void BeginScene(OrthographicCamera& camera);
		static void BeginScene(const Matrix4& viewProjection);
		static void EndScene();

		static void Submit(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const Matrix4& tranform = Matrix4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			Matrix4 ViewProjectionMatrix;
		};



		static SceneData* m_SceneData;
	};
}
