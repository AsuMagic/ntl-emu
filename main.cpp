#include "ntl/cpu.hpp"

#include <cstdint>
#include <fstream>
#include <vector>

std::vector<std::uint8_t> read_file(std::ifstream& file)
{
	file.seekg(0, std::ios::end);
	std::vector<std::uint8_t> program(file.tellg());
	file.seekg(0);
	file.read(reinterpret_cast<char*>(program.data()), program.size()); 
	return program;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: ntlemu [program]\n";
		return -1;
	}
	
	ntl::CPU cpu;
	
	std::ifstream file{argv[1]};
	
	if (!file)
	{
		std::cerr << "Failed to load program from file '" << argv[1] << "'\n";
		return -1;
	}
	
	cpu.memory_move(ntl::Memory<ntl::word_t, 65536>{read_file(file)});

	cpu.run();
}
