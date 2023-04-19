#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "instruction_specification_builder.hpp"
#include "instruction_printer.hpp"

int main(int argc, char *argv[])
{
	const std::string filename = "instructions_specification/instruction_specification_table.txt";
	std::string file_contents = read_entire_file(filename);

	std::vector<SingleInstructionSpecification> instruction_specifcation = parse_instruction_specification(file_contents);

	for (SingleInstructionSpecification& spec: instruction_specifcation) {
		std::cout << to_string(spec) << "\n";
	}

	return 0;
}