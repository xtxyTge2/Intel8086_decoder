#include <iostream>
#include <vector>

#include "io_utils.hpp"
#include "instruction_specification_builder.hpp"
#include "instruction_printer.hpp"
#include "instruction_decoder.hpp"

int main(int argc, char *argv[])
{
	const std::string specification_filename = "C:/Users/kala/code/Intel8086_decoder/instructions_specification/instruction_specification_table.txt";
	std::string specification_file_contents = read_entire_file(specification_filename);

	std::vector<SingleInstructionSpecification> specification = parse_instruction_specification(specification_file_contents);

	/*
	for (SingleInstructionSpecification& instruction_specifcation: specification) {
		std::cout << to_string(instruction_specifcation) << "\n";
	}
	std::cout << "\n\n\n\n";
	*/

	const std::string input_filename = "C:/Users/kala/code/Intel8086_decoder/tests/listing_0040_challenge_movs";
	std::string file_contents = read_entire_file(input_filename);	 
	std::cout << "filename: " << input_filename << "\n";
	for (char v: file_contents) {
		std::cout << char_to_binary_string(v) << " ";
	}
	std::cout << "\n\n";

	DecodingContext decoding_context = {};
	decoding_context.address = 0;
	decoding_context.data = file_contents;
	decoding_context.specification = specification;
	
	std::vector<Instruction> decoded_instructions = decode_instruction_stream(decoding_context);

	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	for (Instruction& instruction: decoded_instructions) {
		std::cout << "Decoded instruction: " << to_string(instruction) << "\n";
	}

	return 0;
}