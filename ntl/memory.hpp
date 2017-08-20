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
		Memory(std::initializer_list<T> list);

		T& operator[](const std::size_t at);
	};
}

#include "memory.inl"

#endif // MEMORY_HPP
