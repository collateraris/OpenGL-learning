#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <cassert>
#include <utility>
#include <vector> // for std::data

namespace System
{
	class Buffer
	{
	public:
		explicit Buffer(GLenum target);

		~Buffer();

		template<typename T, typename = std::void_t<decltype(std::data(std::declval<T>())), typename T::value_type>>
		void SetStorage(const T& container, GLbitfield flags);

		template <typename T>
		void SetContentSubData(const T& data, size_t startOffset);

		void BindBase(unsigned int binding) const;

		GLuint GetHandle() const;

	private:

		GLenum m_Target;
		GLuint m_BufferHandle;
		GLbitfield m_BufferFlags;
		bool bIsImmutable = false;
		size_t m_TypeSize = 0;
	};

	template<typename T, typename/* = std::void_t<decltype(std::data(std::declval<T>())), typename T::value_type>*/>
	void Buffer::SetStorage(const T& container, GLbitfield flags)
	{
		assert(bIsImmutable == false, "Buffer is immutable, cannot reallocate buffer data");
		m_BufferFlags = flags;
		m_TypeSize = sizeof(T::value_type);

		glNamedBufferStorage(m_BufferHandle, container.size() * m_TypeSize, container.data(), m_BufferFlags);
		bIsImmutable = true;
	}

	template <typename T>
	void Buffer::SetContentSubData(const T& data, size_t startOffset)
	{
		glNamedBufferSubData(m_BufferHandle, startOffset, sizeof(data), &data);
	}
}