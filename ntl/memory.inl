#include "memory.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace ntl
{
	template<class T, std::size_t Size>
	Memory<T, Size>::Memory() :
		_memory(Size)
	{}

	template<class T, std::size_t Size>
	Memory<T, Size>::Memory(std::initializer_list<T> list) :
		_memory(Size)
	{
		auto cp_end = end(list);
		
		if (list.size() > Size)
		{
			std::cout << "Memory warning: Given list will be truncated (" << Size << " < " << list.size() << ")\n";
			cp_end = begin(list) + Size;
		}

		std::copy(begin(list), cp_end, begin(_memory));
	}

	template<class T, std::size_t Size>
	T& Memory<T, Size>::operator[](const std::size_t at)
	{
		// NOTE memory[] performs bound-checking
		// memory accesses could be optimized if we had the compile-time guarentee that the memory address exclusively fits into Size.
		return _memory.at(at);
	}
}
