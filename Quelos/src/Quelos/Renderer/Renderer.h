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
		static void BeginScene(const glm::mat4& viewProjection);
		static void EndScene();

		static void Submit(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const glm::mat4& tranform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};



		static SceneData* m_SceneData;
	};
}
