#include <cassert>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>

#include "instruction_specification_builder.hpp"

std::vector < SingleInstructionSpecification > parse_instruction_specification(std::string& input_data) {
	const std::string INSTRUCTION_START_STRING = "[";
	const std::string INSTRUCTION_END_STRING = "]";

	std::stringstream data(input_data);
	
	std::vector<SingleInstructionSpecification> instruction_specification; 
	SingleInstructionSpecification current_instruction_specification;

	bool is_parsing_instruction = false;
	for (std::string line; std::getline(data, line);) {
		if (line.starts_with(INSTRUCTION_START_STRING)) {
			assert(!is_parsing_instruction);
			// can only start an instruction if we are not already inside one
			is_parsing_instruction = true;
			
			// reset instruction spec, since its the start of a new instruction.
			current_instruction_specification = {};

			// parse the mnemonic and debug_name, for this we remove the INSTRUCTION_START string from 
			// the current line and then try to parse the mnemonic and debug name.
			std::string rest_of_line = line.substr(INSTRUCTION_END_STRING.size(), line.size());
			parse_mnemonic_and_debug_name(current_instruction_specification, rest_of_line);
			continue;
		}

		if (line.starts_with(INSTRUCTION_END_STRING)) {
			assert(is_parsing_instruction);
			// can only end an instruction if we are inside one
			is_parsing_instruction = false;

			// at the instruction specification to all specs.
			instruction_specification.push_back(current_instruction_specification);
			continue;
		}
		
		if (!is_parsing_instruction) {
			// if we are not inside an instruction block we ignore this line
			// (except start/end block instructions, but these are handled above.)
			continue;
		}
		// if we reach this line we are currently in an instruction block, so we try 
		// to parse it as an instruction field.
		
		// parse instruction field and add it to current instruction specification.
		// at the moment we dont check for maximum instruction specification length
		InstructionField field = parse_instruction_field(line);
		current_instruction_specification.instruction_fields.push_back(field);
	}

	return instruction_specification;
}


void parse_mnemonic_and_debug_name(SingleInstructionSpecification& specification, std::string data) {
	std::vector<std::string> split = split_string_by_delimiter_and_remove_any_whitespaces(data, ',');
	
	assert(split.size() == 2); // with this we actually specify, that you have to have a debug_name
	
	specification.mnemonic = split[0];
	specification.debug_name = split[1];
}


// @Speed @Cleanup: there are various ways to improve this, but for our use case this just works fine, because
// we just parse a single small .txt file containing the specification of each instruction, 
// to build a specification table. Since building this table could be done on compile time 
// (in theory) we might revisit this anyway.
InstructionField parse_instruction_field(std::string data) {
	InstructionField field = {};

	std::vector<std::string> split_data = split_string_by_delimiter_and_remove_any_whitespaces(data, ',');
	

	field.type = InstructionFieldTypes::UNKNOWN;
	if (split_data[0] == "LITERAL") {
		field.type = InstructionFieldTypes::LITERAL;
		assert(split_data.size() == 3);
		field.length_in_bits = std::stoi(split_data[1]);
		field.binary_string = std::stoi(split_data[2]);
		field.value = std::stoi(split_data[2], nullptr, 2);
	}
	else if (split_data[0] == "MOD") {
		field.type = InstructionFieldTypes::MOD;
	}
	else if (split_data[0] == "REG") {
		field.type = InstructionFieldTypes::REG;
	}
	else if (split_data[0] == "RM") {
		field.type = InstructionFieldTypes::RM;
	}
	else if (split_data[0] == "SR") {
		field.type = InstructionFieldTypes::SR;
	}
	else if (split_data[0] == "D_BIT") {
		field.type = InstructionFieldTypes::D_BIT;
	}
	else if (split_data[0] == "W_BIT") {
		field.type = InstructionFieldTypes::W_BIT;
	}
	else if (split_data[0] == "S_BIT") {
		field.type = InstructionFieldTypes::S_BIT;
	}
	else if (split_data[0] == "V_BIT") {
		field.type = InstructionFieldTypes::V_BIT;
	}
	else if (split_data[0] == "DISPLACEMENT_LOW") {
		field.type = InstructionFieldTypes::DISPLACEMENT_LOW;
	}
	else if (split_data[0] == "DISPLACEMENT_HIGH") {
		field.type = InstructionFieldTypes::DISPLACEMENT_HIGH;
	}
	else if (split_data[0] == "DATA") {
		field.type = InstructionFieldTypes::DATA;
	}
	else if (split_data[0] == "DATA_EXTENDED") {
		field.type = InstructionFieldTypes::DATA_EXTENDED;
	}
	else if (split_data[0] == "DATA_IF_W") {
		field.type = InstructionFieldTypes::DATA_IF_W;
	}
	else if (split_data[0] == "DATA_IF_SW") {
		field.type = InstructionFieldTypes::DATA_IF_SW;
	}
	else if (split_data[0] == "ADDRESS_LOW") {
		field.type = InstructionFieldTypes::ADDRESS_LOW;
	}
	else if (split_data[0] == "ADDRESS_HIGH") {
		field.type = InstructionFieldTypes::ADDRESS_HIGH;
	}
	else if (split_data[0] == "DATA_8") {
		field.type = InstructionFieldTypes::DATA_8;
	} else {
		// Leave this for the moment, so we know when we forgot to implement another field type.
		assert(false);
	}
	
	if (field.type != InstructionFieldTypes::LITERAL) {
		// sanity check, make sure we just got a single token on a non literal line, inside an instruction block.
		assert(split_data.size() == 1);
	}
	
	// @Cleanup: Replace this with a compile time built lookup table. For now this is fine. 
	std::array < int, InstructionFieldTypes::COUNT > instruction_field_length_lookup_table = {};
	instruction_field_length_lookup_table[UNKNOWN] = -1; // dummy value, should never be accessed.
	instruction_field_length_lookup_table[LITERAL] = -1; // dummy value, should never be accessed.
	instruction_field_length_lookup_table[MOD] = 2;
	instruction_field_length_lookup_table[REG] = 3;
	instruction_field_length_lookup_table[RM] = 3;
	instruction_field_length_lookup_table[SR] = 2;
	instruction_field_length_lookup_table[D_BIT] = 1;
	instruction_field_length_lookup_table[W_BIT] = 1;
	instruction_field_length_lookup_table[S_BIT] = 1;
	instruction_field_length_lookup_table[V_BIT] = 1;
	instruction_field_length_lookup_table[DISPLACEMENT_LOW] = 8;
	instruction_field_length_lookup_table[DISPLACEMENT_HIGH] = 8;
	instruction_field_length_lookup_table[DATA] = 8;
	instruction_field_length_lookup_table[DATA_EXTENDED] = 8;
	instruction_field_length_lookup_table[DATA_IF_W] = 8;
	instruction_field_length_lookup_table[DATA_IF_SW] = 8;
	instruction_field_length_lookup_table[ADDRESS_LOW] = 8;
	instruction_field_length_lookup_table[ADDRESS_HIGH] = 8;
	instruction_field_length_lookup_table[DATA_8] = 8;

	// LITERAL is the only case, where the length is not known beforehand and gets parsed above. For the rest we lookup the length of the data field in a lookup table.
	if (field.type != InstructionFieldTypes::LITERAL) {
		field.length_in_bits = instruction_field_length_lookup_table[field.type];
	}

	return field;
}


std::vector<std::string> split_string_by_delimiter_and_remove_any_whitespaces(std::string data, char delimiter) {
	std::vector < std::string > string_split;

	std::stringstream data_stream(data);
	std::string str;

	while(std::getline(data_stream, str, delimiter))
	{
		// remove any whitespace before adding it to the list
		str.erase(remove_if(str.begin(), str.end(), isspace), str.end());

		string_split.push_back(str);
	}
	return string_split;
}
