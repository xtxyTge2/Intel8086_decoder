#include <iostream>
#include <vector>

#include "io_utils.hpp"
#include "instruction_specification_builder.hpp"
#include "instruction_printer.hpp"
#include "instruction_decoder.hpp"

int main()
{
	const std::string specification_filename = "C:/Users/kala/code/Intel8086_decoder/instructions_specification/instruction_specification_table.txt";
	std::string specification_file_contents = read_entire_file(specification_filename);

	std::vector<SingleInstructionSpecification> specification = parse_instruction_specification(specification_file_contents);

	const std::string input_filename = "C:/Users/kala/code/Intel8086_decoder/tests/listing_0040_challenge_movs";
	std::string file_contents = read_entire_file(input_filename);	 
	std::cout << "filename: " << input_filename << "\n";
	for (char v: file_contents) {
		std::cout << char_to_binary_string(static_cast<unsigned char>(v)) << " ";
	}
	std::cout << "\n\n";

	std::vector<uint8_t> data;
	data.reserve(file_contents.size());
	for(char c: file_contents) {
		data.push_back(static_cast<uint8_t>(c));
	}



	DecodingContext decoding_context = {};
	decoding_context.address = 0;
	decoding_context.data = data;
	decoding_context.specification = specification;
	
	std::vector<Instruction> decoded_instructions = decode_instruction_stream(decoding_context);

	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	for (Instruction& instruction: decoded_instructions) {
		std::cout << "Decoded instruction: " << to_string(instruction) << "\n";
	}


	return 0;
}