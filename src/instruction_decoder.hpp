#pragma once

#include <string>
#include <vector>
#include <array>
#include "instruction_specification_builder.hpp"


struct InstructionInfo {
	uint32_t address;
	uint8_t offset_in_bytes; // offset = 0 means its 1 byte long, and has the value of address byte.
	bool is_valid;
	SingleInstructionSpecification matching_specification;
	std::array<uint8_t, InstructionFieldTypes::COUNT> fields_data;
	std::vector<uint8_t> raw_data;

	// @TODO: Replace this with a flags variable.
	std::array<bool, InstructionFieldTypes::COUNT> already_set_fields_array;
};


struct Operand {
	std::string repr;
};


struct Instruction {
	int address;
	std::string mnemonic;
	Operand source_operand;
	Operand destination_operand;

	bool is_wide;
};


struct DecodingContext {
	int address;
	std::vector<uint8_t> data;
	std::vector<SingleInstructionSpecification> specification;
};


InstructionInfo fill_out_next_instruction_info(DecodingContext& decoding_context);
InstructionInfo try_to_find_matching_instruction_specification(DecodingContext& decoding_context, const SingleInstructionSpecification& instruction_spec);
std::vector<Instruction> decode_instruction_stream(DecodingContext& decoding_context);
Instruction decode_next_instruction_and_update_context(DecodingContext& decoding_context);
Instruction convert_info_to_instruction(const InstructionInfo& info);
std::string lookup_register_name(bool is_wide, uint8_t lookup_value);
uint8_t assert_field_has_been_set_and_get_value(const InstructionInfo& info, InstructionFieldTypes field_type);

uint8_t read_bits_in_highest_to_lowest_order_from_byte(uint8_t byte, uint8_t position, uint8_t number_of_bits_to_read);

