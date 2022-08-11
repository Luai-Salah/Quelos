#include "QSPCH.h"
#include "OpenGLVertexArray.h"

#include <glad\glad.h>

namespace Quelos
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Quelos::ShaderDataType::Float:		return GL_FLOAT;
			case Quelos::ShaderDataType::Vector2:		return GL_FLOAT;
			case Quelos::ShaderDataType::Vector3:		return GL_FLOAT;
			case Quelos::ShaderDataType::Vector4:		return GL_FLOAT;
			case Quelos::ShaderDataType::Int:			return GL_INT;
			case Quelos::ShaderDataType::Vector2Int:	return GL_INT;
			case Quelos::ShaderDataType::Vector3Int:	return GL_INT;
			case Quelos::ShaderDataType::Vector4Int:	return GL_INT;
			case Quelos::ShaderDataType::Mat3:			return GL_FLOAT;
			case Quelos::ShaderDataType::Mat4:			return GL_FLOAT;
			case Quelos::ShaderDataType::Bool:			return GL_BOOL;

			default: QS_CORE_ASSERT(false, "Unknown ShaderDataType!"); return 0;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		QS_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		QS_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		QS_PROFILE_FUNCTION();
	
		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::UnBind() const
	{
		QS_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		QS_PROFILE_FUNCTION();

		QS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& e : layout)
		{
			glEnableVertexAttribArray(index);

			switch (e.Type)
			{
				case ShaderDataType::Int:
				{
					glVertexAttribIPointer(
						index,
						e.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(e.Type),
						layout.GetStride(),
						(const void*)(uint64_t)e.Offset
					);
					break;
				}
				default:
				{
					glVertexAttribPointer(
						index,
						e.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(e.Type),
						e.Normlize ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(uint64_t)e.Offset
					);
					break;
				}
			}

			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		QS_PROFILE_FUNCTION();
	
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}
