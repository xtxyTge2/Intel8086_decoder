#define _USE_MATH_DEFINES
#include <cmath>
#include "instruction_decoder.hpp"
#include "instruction_printer.hpp"
#include "io_utils.hpp"

#include "gtest/gtest.h"
namespace {

	TEST(instruction_decoder_test, listing_0040_challenge_movs) 
	{
		const std::string specification_filename = "C:/Users/kala/code/Intel8086_decoder/instructions_specification/instruction_specification_table.txt";
		std::string specification_file_contents = read_entire_file(specification_filename);

		std::vector<SingleInstructionSpecification> specification = parse_instruction_specification(specification_file_contents);

		const std::string input_filename = "C:/Users/kala/code/Intel8086_decoder/tests/listing_0040_challenge_movs";
		std::string file_contents = read_entire_file(input_filename);	 

		std::vector<uint8_t> data;
		data.reserve(file_contents.size());
		for (int v: file_contents) {
			data.push_back(static_cast<uint8_t>(v));
		}

		DecodingContext decoding_context = {};
		decoding_context.address = 0;
		decoding_context.data = data;
		decoding_context.specification = specification;
	
		std::vector<Instruction> decoded_instructions = decode_instruction_stream(decoding_context);

		EXPECT_EQ(1.0, 1.0);
	}
} // namespace

//std::vector<int> input_as_binary_literals = {0b10001011, 0b01000001, 0b11011011, 0b10001001, 0b10001100, 0b11010100, 0b11111110, 0b10001011, 0b01010111, 0b11100000, 0b11000110, 0b00000011, 0b00000111, 0b11000111, 0b10000101, 0b10000101, 0b00000011, 0b01011011, 0b00000001, 0b10001011, 0b00101110, 0b00000101, 0b00000000, 0b10001011, 0b00011110, 0b10000010, 0b00001101, 0b10100001, 0b11111011, 0b00001001, 0b10100001, 0b00010000, 0b00000000, 0b10100011, 0b11111010, 0b00001001, 0b10100011, 0b00001111, 0b00000000};