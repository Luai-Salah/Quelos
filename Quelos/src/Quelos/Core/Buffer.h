#pragma once

#include <cstdint>
#include <memory>

namespace Quelos
{
	// Non-owning raw buffer
	struct Buffer
	{
		Buffer() = default;
		Buffer(const Buffer&) = default;

		Buffer(uint64_t size)
		{
			Allocate(size);
		}

		uint8_t* Data() const { return m_Data; }
		uint64_t Size() const { return m_Size; }

		static Buffer Copy(const Buffer& other)
		{
			Buffer result(other.m_Size);
			memcpy(result.m_Data, other.m_Data, other.m_Size);
		}

		void Allocate(uint64_t size)
		{
			Release();

			m_Data = new uint8_t[size];
			m_Size = size;
		}
		
		void Release()
		{
			delete[] m_Data;
			m_Data = nullptr;
			m_Size = 0;
		}

		template<typename T>
		T* As()
		{
			return (T*)m_Data;
		}

		operator bool() const
		{
			return (bool)m_Data;
		}
	private:
		uint8_t* m_Data = nullptr;
		uint64_t m_Size = 0;
	};

	struct ScopedBuffer
	{
		ScopedBuffer(const Buffer& buffer)
			: m_Buffer(buffer)
		{
		}

		ScopedBuffer(uint64_t size)
			: m_Buffer(size)
		{
		}

		~ScopedBuffer()
		{
			m_Buffer.Release();
		}

		uint8_t* Data() const { return m_Buffer.Data(); }
		uint64_t Size() const { return m_Buffer.Size(); }

		template<typename T>
		T* As()
		{
			return m_Buffer.As<T>();
		}

		operator bool() const { return m_Buffer; }
	private:
		Buffer m_Buffer;
	};
}
