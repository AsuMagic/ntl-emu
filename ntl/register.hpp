#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <array>

#include "cputypes.hpp"

namespace ntl
{
	struct RegisterInfo
	{
		const char* const name;
	};

	struct Register
	{
		word_t value = 0;

		operator word_t() const;
		operator word_t&();
		Register& operator=(const word_t newvalue);
	};
	
	struct Flag
	{
		word_t& value;
		const word_t mask;
		
		operator bool() const;
		Flag& operator=(const bool newvalue);
	};

	struct FlagRegister : Register
	{
		Flag inton  {value, 1 << 0x0},
		     int1on {value, 1 << 0x1},
			 int2on {value, 1 << 0x2},
			 int3on {value, 1 << 0x3},
			 int4on {value, 1 << 0x4},
			 int5on {value, 1 << 0x5},
			 int6on {value, 1 << 0x6},
			 int7on {value, 1 << 0x7},
			 test   {value, 1 << 0x8},
			 iofail {value, 1 << 0x9},
			 intlock{value, 1 << 0xA};

		Flag& operator[](const std::size_t at);
	};

	struct Registers
	{
		constexpr static std::array<RegisterInfo, 8> infos =
		{{
			{"rfl"},
			{"ridt"},
			{"racc"},
			{"rsp"},
			{"r4"},
			{"r5"},
			{"r6"},
			{"r7"}
		}};

		FlagRegister rfl;
		Register ridt, racc, rsp, r4, r5, r6, r7;
		Register rip; // Special register, not shown to the program

		Register& operator[](const std::size_t at);
	};
}

#endif // REGISTER_HPP
