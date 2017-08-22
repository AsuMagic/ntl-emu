#include "memory.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstring>

namespace ntl
{
	template<class T, std::size_t Size>
	Memory<T, Size>::Memory() :
		_memory(Size)
	{}
	
	template<class T, std::size_t Size>
	template<class U>
	Memory<T, Size>::Memory(U c) :
		_memory(Size)
	{
		// TODO just make this less terrible
		if (static_cast<std::size_t>(c.size()) > (Size * sizeof(instruction_t)))
		{
			std::cout << "Memory warning: Given memory range will be truncated (" << Size << " < " << c.size() << ")\n";
			c.resize(Size * sizeof(instruction_t));
		}
		
		std::memcpy(_memory.data(), &(c[0]), c.size());
	}

	template<class T, std::size_t Size>
	T& Memory<T, Size>::operator[](const std::size_t at)
	{
		// NOTE memory[] performs bound-checking
		// memory accesses could be optimized if we had the compile-time guarentee that the memory address exclusively fits into Size.
		return _memory.at(at);
	}
}
