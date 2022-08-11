#pragma once

namespace Quelos
{
	enum class ShaderDataType
	{
		None = 0,
		Float, Vector2, Vector3, Vector4,
		Int, Vector2Int, Vector3Int, Vector4Int,
		Mat3, Mat4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:				break;
			case ShaderDataType::Float:				return 4;
			case ShaderDataType::Vector2:			return 4 * 2;
			case ShaderDataType::Vector3:			return 4 * 3;
			case ShaderDataType::Vector4:			return 4 * 4;

			case ShaderDataType::Int:				return 4;
			case ShaderDataType::Vector2Int:		return 4 * 2;
			case ShaderDataType::Vector3Int:		return 4 * 3;
			case ShaderDataType::Vector4Int:		return 4 * 4;

			case ShaderDataType::Mat3:				return 4 * 3 * 3;
			case ShaderDataType::Mat4:				return 4 * 4 * 4;
			case ShaderDataType::Bool:				return 1;

			default: QS_CORE_ASSERT(false, "Unknown ShaderDataType!"); return 0;
		}

		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normlize;

		BufferElement(ShaderDataType type, const std::string& name, bool normlize = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(Type)), Offset(0), Normlize(normlize)
		{
		}

		BufferElement() {}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case Quelos::ShaderDataType::None:			return 0;
				case Quelos::ShaderDataType::Float:		return 1;
				case Quelos::ShaderDataType::Vector2:		return 2;
				case Quelos::ShaderDataType::Vector3:		return 3;
				case Quelos::ShaderDataType::Vector4:		return 4;
				case Quelos::ShaderDataType::Int:			return 1;
				case Quelos::ShaderDataType::Vector2Int:	return 2;
				case Quelos::ShaderDataType::Vector3Int:	return 3;
				case Quelos::ShaderDataType::Vector4Int:	return 4;
				case Quelos::ShaderDataType::Mat3:			return 3 * 3;
				case Quelos::ShaderDataType::Mat4:			return 4 * 4;
				case Quelos::ShaderDataType::Bool:			return 1;
				default:								QS_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}

			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline uint32_t GetStride() const { return m_Stride; }

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& e : m_Elements)
			{
				e.Offset = offset;
				offset += e.Size;
				m_Stride += e.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}
