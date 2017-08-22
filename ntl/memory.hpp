#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstddef>
#include <vector>
#include <initializer_list>

#include "cputypes.hpp"

namespace ntl
{
	template<class T, std::size_t Size>
	class Memory
	{
		std::vector<T> _memory;

	public:
		Memory();
		
		template<class U>
		Memory(U c); // copy from contiguous container

		T& operator[](const std::size_t at);
	};
}

#include "memory.inl"

#endif // MEMORY_HPP
