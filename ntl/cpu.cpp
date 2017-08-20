#include "cpu.hpp"

#include <cstddef>
#include <functional>
#include <iostream>
#include <iomanip>

namespace ntl
{
	Instruction::Instruction() :
		op{[](CPU& cpu, const instruction_t) {
			cpu.exception(0x0001, "Unknown opcode");
		}}
	{}
	
	Instruction::Instruction(const char* const _opname, interpreter _op) :
		opname{_opname}, op{_op}
	{}
	
	template<class T> struct bit_rightshift  { constexpr T operator()(const T a, const T b) { return a >> b; } };
	template<class T> struct bit_leftshift   { constexpr T operator()(const T a, const T b) { return a << b; } };
	template<class T> struct emu_arightshift { constexpr T operator()(const T a, const T b) { /*return a >> b;{"ashr not implemented", 0x0002};*/return a >> b; } };
	template<class T> struct emu_bit_get     { constexpr T operator()(const T a, const T offset) { return (a >> offset) & 0b1; } };

	template<class T> struct emu_equal_to_zero { constexpr T operator()(const T a) { return a == 0; } };
	
	template<class F>
	Instruction::interpreter Instruction::arithmetic_binop()
	{
		return [](CPU& cpu, const instruction_t ins)
		{
			const Register& ra = cpu.reg(slice<8, 4>(ins));
			const Register& rb = cpu.reg(slice<12, 4>(ins));
			auto& rdst = cpu.reg(slice<16, 4>(ins));
			rdst = F()(ra.value, rb.value);
		};
	}

	template<class F>
	Instruction::interpreter Instruction::arithmetic_unop()
	{
		return [](CPU& cpu, const instruction_t ins)
		{
			const Register& ra = cpu.reg(slice<8, 4>(ins));
			Register& rdst = cpu.reg(slice<12, 4>(ins));
			rdst = F()(ra.value);
		};
	}
	
	template<class F>
	Instruction::interpreter Instruction::test_binop()
	{
		return [](CPU& cpu, const instruction_t ins)
		{
			const Register& ra = cpu.reg(slice<8, 4>(ins));
			const Register& rb = cpu.reg(slice<12, 4>(ins));
			
			if (F()(ra.value, rb.value))
				cpu.rfl.test = true;
		};
	}
	
	template<class F>
	Instruction::interpreter Instruction::test_unop()
	{
		return [](CPU& cpu, const instruction_t ins)
		{
			const Register& ra = cpu.reg(slice<8, 4>(ins));
			
			if (F()(ra.value))
				cpu.rfl.test = true;
		};
	}

	const std::array<Instruction, 256> CPU::_instruction_set =
	{{
		{"nop", [](CPU&, const instruction_t) {}},
		{"load", [](CPU& cpu, const instruction_t ins) {
			auto& rdst = cpu.reg(slice<8, 4>(ins));
			const auto& smem = cpu.smem(slice<16, 16>(ins));
			rdst = smem;
		}},
		{"store", [](CPU& cpu, const instruction_t ins) {
			const auto& rsrc = cpu.reg(slice<8, 4>(ins));
			auto& smem = cpu.smem(slice<16, 16>(ins));
			smem = rsrc;
		}},
		{"pload", [](CPU& cpu, const instruction_t ins) {
			auto& rdst = cpu.reg(slice<8, 4>(ins));
			const auto& pmem = cpu.pmem(slice<16, 16>(ins));
			rdst = pmem;
		}},
		{"pstore", [](CPU& cpu, const instruction_t ins) {
			const auto& rsrc = cpu.reg(slice<8, 4>(ins));
			auto& pmem = cpu.pmem(slice<16, 16>(ins));
			pmem = rsrc;
		}},
		{"mov", [](CPU& cpu, const instruction_t ins) {
			const auto& rsrc = cpu.reg(slice<8, 4>(ins));
			auto& rdst = cpu.reg(slice<12, 4>(ins));
			rdst = rsrc;
		}},
		{"add", Instruction::arithmetic_binop<std::plus      <word_t>>()},
		{"sub", Instruction::arithmetic_binop<std::minus     <word_t>>()},
		{"mul", Instruction::arithmetic_binop<std::multiplies<word_t>>()},
		{"div", [](CPU& cpu, const instruction_t ins) {
			const auto& ra = cpu.reg(slice<8, 4>(ins));
			const auto& rb = cpu.reg(slice<12, 4>(ins));
			auto& rdst = cpu.reg(slice<16, 4>(ins));
			
			if (rb == 0)
				cpu.exception(0x0000, "Illegal arithmetic: Division by zero");
			else
				rdst = ra / rb;
		}},
		{"and",  Instruction::arithmetic_binop<std::bit_and   <word_t>>()},
		{"or",   Instruction::arithmetic_binop<std::bit_or    <word_t>>()},
		{"xor",  Instruction::arithmetic_binop<std::bit_xor   <word_t>>()},
		{"not",  Instruction::arithmetic_unop <std::bit_not   <word_t>>()},
		{"shl",  Instruction::arithmetic_binop<bit_leftshift  <word_t>>()},
		{"shr",  Instruction::arithmetic_binop<bit_rightshift <word_t>>()},
		{"ashr", Instruction::arithmetic_binop<emu_arightshift<word_t>>()},
		{"gbit", Instruction::arithmetic_binop<emu_bit_get    <word_t>>()},
		{"fbit", [](CPU& cpu, const instruction_t ins) {
			auto& ra = cpu.reg(slice<8, 4>(ins));
			const auto& roff = cpu.reg(slice<12, 4>(ins));
			ra = ra ^ static_cast<word_t>(0b1 << roff);
		}},
		{"pop", [](CPU& cpu, const instruction_t ins) {
			auto& rdst = cpu.reg(slice<8, 4>(ins));
			rdst = cpu._scratchpad[cpu.rsp.value--];
		}},
		{"push", [](CPU& cpu, const instruction_t ins) {
			const auto rsrc = cpu.reg(slice<8, 4>(ins));
			cpu._scratchpad[++cpu.rsp.value] = rsrc;
		}},
		{"jmpi", [](CPU& cpu, const instruction_t ins) {
			const auto iaddr = slice<16, 16>(ins);
			cpu.rip = static_cast<word_t>(iaddr);
		}},
		{"jmp", [](CPU& cpu, const instruction_t ins) {
			const auto raddr = cpu.reg(slice<8, 4>(ins));
			cpu.rip = static_cast<word_t>(raddr);
		}},
		{"cjmpi", [](CPU& cpu, const instruction_t ins) {
			const auto iaddr = slice<16, 16>(ins);
			if (cpu.rfl.test)
				cpu.rip = static_cast<word_t>(iaddr);
		}},
		{"cjmp", [](CPU& cpu, const instruction_t ins) {
			const auto raddr = cpu.reg(slice<8, 4>(ins));
			if (cpu.rfl.test)
				cpu.rip = raddr;
		}},
		{"ret", [](CPU& cpu, const instruction_t) {
			cpu.rip = cpu._scratchpad[cpu.rsp.value--];
		}},
		{"calli", [](CPU& cpu, const instruction_t ins) {
			const auto iaddr = slice<16, 16>(ins);
			cpu._scratchpad[++cpu.rsp.value] = cpu.rip;
			cpu.rip = iaddr;
		}},
		{"call", [](CPU& cpu, const instruction_t ins) {
			const auto& raddr = cpu.reg(slice<8, 4>(ins));
			cpu._scratchpad[++cpu.rsp.value] = cpu.rip;
			cpu.rip = raddr;
		}},
		{"tz",  Instruction::test_unop <emu_equal_to_zero <word_t>>()},
		{"tht", Instruction::test_binop<std::greater      <word_t>>()},
		{"thq", Instruction::test_binop<std::greater_equal<word_t>>()},
		{"teq", Instruction::test_binop<std::equal_to     <word_t>>()},
		{"ldi", [](CPU& cpu, const instruction_t ins) {
			auto& rdst = cpu.reg(slice<8, 4>(ins));
			const auto iaddr = slice<16, 16>(ins);
			rdst = iaddr;
		}},
		{"hlt", [](CPU& cpu, const instruction_t) {
			cpu.exception(0x0002, "CPU halted. No interrupts."); // TODO use interrupts for hlt
		}},
		{"read", [](CPU& cpu, const instruction_t) {
			cpu.exception(0x0002, "Port I/O not implemented.");
		}},
		{"write", [](CPU& cpu, const instruction_t) {
			cpu.exception(0x0002, "Port I/O not implemented.");
		}},
		{"wait", [](CPU& cpu, const instruction_t) {
			cpu.exception(0x0002, "Port I/O not implemented.");
		}},
		{"int", [](CPU& cpu, const instruction_t ins) {
			const auto iid = slice<16, 16>(ins);
			cpu.interrupt(iid);
		}}
	}};

	void CPU::program_move(Memory<program_t, 65536>&& program)
	{
		_program = std::move(program);
	}

	Register& CPU::reg(const instruction_t id)
	{
		return (*this)[id];
	}

	word_t& CPU::smem(const instruction_t at)
	{
		return _scratchpad[at];
	}

	program_t& CPU::pmem(const instruction_t at)
	{
		return _program[at];
	}

	void CPU::run()
	{
		while (!_abort)
		{
			instruction_t instruction = _program[rip] | (_program[rip.value + 1] << 16); // Instructions are 32-bit, so they are split accross 2 CPU words.
			const std::uint8_t opcode = instruction & 0xFF; // The opcode is located on the first 8 bits of the instruction.
			
#ifdef NTL_DEBUG
			std::cerr << "0x" << std::hex << std::setfill('0') << std::setw(4) << rip << ": 0x" << std::setw(8) << instruction << " -> '" << _instruction_set[opcode].opname << "'... ";
#endif
			
			rip.value += 2;

			_instruction_set[opcode].op(*this, instruction); // Call the lambda interpreting this instruction.
#ifdef NTL_DEBUG
			std::cerr << "OK\n";
#endif
		}
		
	}
	
	void CPU::exception(const word_t id, const char *const message)
	{
		std::cerr << "\nEncountered CPU exception id 0x" << std::setfill('0') << std::setw(4) << id << ": '" << message << "'\n";
		
		_scratchpad[++rsp.value] = id; // Push the exception ID to the stack
		interrupt(0); // Issue the interrupt 0 (CPU exception interrupt).
	}
	
	void CPU::interrupt(const std::uint8_t id)
	{
		if (id == 0) // Handle special cases for CPU exceptions.
		{
			if (!rfl.inton) // The exception interrupt cannot be disabled, else it will cause the CPU to halt.
			{
				std::cerr << "A CPU exception interrupt was issued, but the CPU program did not enable interrupts globally. Halting.\n";
				_abort = true;
				return;
			}
		}
		else
		{		
			if (!rfl[0x1 + (id - 1)]) // Check for the int*on flags.
			{
#ifdef NTL_DEBUG
				std::cerr << "\nDiscarding disabled interrupt '" << std::hex << +id << "'\n";
#endif
				return;
			}
		}
		
		if (rfl.intlock)
		{
			std::cerr << "An interrupt was issued inside of an interrupt handler. Halting.\n";
			_abort = true;
			return;
		}
		
		const word_t handler_address = ridt + (id * 2);
		_scratchpad[++rsp.value] = rip;
		rip = handler_address; // Call the interrupt handler.
		
		rfl.intlock = true;
		
#ifdef NTL_DEBUG
		std::cerr << "\nEntering interrupt handler " << std::hex << +id << " at 0x" << std::setw(4) << handler_address << ".\n";
#endif
	}
}
