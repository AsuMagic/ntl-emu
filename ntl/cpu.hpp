#ifndef CPU_HPP
#define CPU_HPP

#include <functional>

#include "cputypes.hpp"
#include "register.hpp"
#include "memory.hpp"

namespace ntl
{
	class CPU;

	template<std::size_t begin, std::size_t end>
	instruction_t slice(const instruction_t data);

	struct Instruction
	{
		using interpreter = std::function<void(CPU&, const instruction_t)>;
		
		Instruction(); // Default constructor, with an illegal operator instruction
		Instruction(const char* const _opname, interpreter _op);

		const char* opname = "_ILLOP";
		interpreter op;

		template<class F> static interpreter arithmetic_binop();
		template<class F> static interpreter arithmetic_unop();
		
		template<class F> static interpreter test_binop();
		template<class F> static interpreter test_unop();
	};

	class CPU : public Registers
	{
		Memory<word_t, 65536> _memory;
		const static std::array<Instruction, 256> _instruction_set;
		
		bool _abort = false;

	public:
		void memory_move(Memory<word_t, 65536>&& init);

		Register& reg(const std::size_t id);
		word_t& mem(const word_t at);

		void run();
		
		void exception(const word_t id, const char *const message = "Unspecified message");
		void interrupt(const std::uint8_t id);
	};
}

#include "cpu.inl"

#endif // CPU_HPP
