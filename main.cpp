#include "ntl/cpu.hpp"

int main()
{
	ntl::CPU cpu;

	cpu.program_move(ntl::Memory<ntl::program_t, 65536>
	{
		0x0220, 0x0005, // ldi racc, 5
		0x0420, 0x0008, // ldi rg0, 8
		0x4206, 0x0002, // add racc, rg0, racc
		0x0012, 0x0000, // fbit rfl, 0; enable interrupts
		0x00FF, 0x0000, // INVALID opcode
	});

	cpu.run();
}
