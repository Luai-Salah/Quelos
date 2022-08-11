#include "QSPCH.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Platform\OpenGL\OpenGLShader.h"
#include "Quelos\Renderer\Renderer2D.h"
#include "Quelos/Renderer/UniformBuffer.h"

#include <glm/gtc/type_ptr.hpp>

namespace Quelos
{
	Renderer::SceneData* Renderer::m_SceneData = new SceneData();

	struct CameraData
	{
		Matrix4 ViewProjection;
	};

	Ref<UniformBuffer> CameraUniformBuffer;
	Ref<UniformBuffer> TransformUniformBuffer;

	void Renderer::Init()
	{
		QS_PROFILE_FUNCTION();

		CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
		TransformUniformBuffer = UniformBuffer::Create(sizeof(Matrix4), 1);

		RenderCommand::Init();
		Renderer2D::Init(CameraUniformBuffer);
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::BeginScene(const Matrix4& viewProjection)
	{
		m_SceneData->ViewProjectionMatrix = viewProjection;
		CameraUniformBuffer->SetData(glm::value_ptr(viewProjection), sizeof(CameraData));
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const Matrix4& tranform)
	{
		shader->Unbind();
		shader->Bind();

		TransformUniformBuffer->SetData(glm::value_ptr(tranform), sizeof(Matrix4));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
