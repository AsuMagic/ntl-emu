#include "cpu.hpp"

namespace ntl
{
	template<std::size_t begin, std::size_t count>
	instruction_t slice(const instruction_t data)
	{
		const instruction_t mask = ((1 << count) - 1) << begin;
		return (data & mask) >> begin;
	}
}
