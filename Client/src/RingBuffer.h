#pragma once

#include "pch.h"
#include "PjState.h"

namespace rmkl {

	class RingBuffer
	{
	public:
		RingBuffer() : m_Head(0), m_Tail(0), m_Size(16), m_Elements() {}

		// add to the end of the list
		void Push(PjState& value)
		{
			ASSERT((m_Tail + 1) % m_Size != m_Head, "Overflow");

			m_Elements[m_Tail] = std::move(value);
			m_Tail = (m_Tail + 1) % m_Size;
		}

		// pop first element
		void Pop()
		{
			ASSERT(m_Head != m_Tail, "Empty");

			//PjState value = m_Elements[m_Head];
			m_Head = (m_Head + 1) % m_Size;
			//return value;
		}

		int Count() {
			int end = m_Tail > m_Head ? m_Tail + m_Size : m_Tail;
			return end - m_Tail;
		}

		inline int Size() { return m_Size; }

		PjState& at(int index) {
			ASSERT(index < Count() && index > 0, "Out of bounds");
			return m_Elements[(m_Tail + index) % m_Size];
		}

	private:
		int m_Head;
		int m_Tail;
		const int m_Size = 16;
		PjState* m_Elements;
	};

}