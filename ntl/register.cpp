#include "register.hpp"

namespace ntl
{
	Register::operator word_t() const
	{
		return value;
	}

	Register::operator word_t&()
	{
		return value;
	}

	Register& Register::operator=(const word_t newvalue)
	{
		value = newvalue;
		return *this;
	}
	
	Flag::operator bool() const
	{
		return value & mask;
	}
	
	Flag& Flag::operator=(const bool newvalue)
	{
		if (newvalue)
			value |= mask;
		else
			value &= ~mask;
		
		return *this;
	}
	
	Flag& FlagRegister::operator[](const std::size_t at)
	{
		static std::array<Flag*, 16> flagarray = {{ &inton, &int1on, &int2on, &int3on, &int4on, &int5on, &int6on, &int7on, &test, &iofail, &intlock }};
		return *(flagarray[at]);
	}

	Register& Registers::operator[](const std::size_t at)
	{
		static std::array<Register*, 8> regarray = {{ &rfl, &ridt, &racc, &rsp, &r4, &r5, &r6, &r7 }};
		return *(regarray[at]);
	}
}
