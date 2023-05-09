#pragma once

#include <string>
#include <vector>


enum InstructionFieldTypes {
	UNKNOWN,
	LITERAL,
	MOD,
	REG,
	RM,
	SR,
	D_BIT,
	W_BIT,
	S_BIT,
	V_BIT,
	DISPLACEMENT_LOW,
	DISPLACEMENT_HIGH,
	DATA,
	DATA_EXTENDED,
	DATA_IF_W, 
	DATA_IF_SW,
	ADDRESS_LOW,
	ADDRESS_HIGH,
	DATA_8,

	COUNT
};


struct InstructionField {
	InstructionFieldTypes type;
	uint8_t length_in_bits;
	int binary_string;
	int value; // either a binary literal, if field_type is InstructionFieldTypes::LITERAL, or its a count value, specifying the size of the field in bits. 
};


struct SingleInstructionSpecification {
	std::string mnemonic;
	std::string debug_name;
	uint8_t length_instruction_fields;
	std::vector < InstructionField> instruction_fields;
};


std::vector<std::string> split_string_by_delimiter_and_remove_any_whitespaces(std::string data, char delimiter);
void parse_mnemonic_and_debug_name(SingleInstructionSpecification& specification, std::string line);
InstructionField parse_instruction_field(std::string data);
std::vector<SingleInstructionSpecification> parse_instruction_specification(std::string& input_data);