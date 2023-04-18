#include <cassert>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>

#include "instruction_specification_builder.hpp"


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
	if (split_data[0] == "MOD") {
		field.type = InstructionFieldTypes::MOD;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 2;
	}
	if (split_data[0] == "REG") {
		field.type = InstructionFieldTypes::REG;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 3;
	}
	if (split_data[0] == "RM") {
		field.type = InstructionFieldTypes::RM;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 3;
	}
	if (split_data[0] == "SR") {
		field.type = InstructionFieldTypes::SR;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 3;
	}
	if (split_data[0] == "D_BIT") {
		field.type = InstructionFieldTypes::D_BIT;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 1;
	}
	if (split_data[0] == "W_BIT") {
		field.type = InstructionFieldTypes::W_BIT;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 1;
	}
	if (split_data[0] == "S_BIT") {
		field.type = InstructionFieldTypes::S_BIT;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 1;
	}
	if (split_data[0] == "V_BIT") {
		field.type = InstructionFieldTypes::V_BIT;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 1;
	}
	if (split_data[0] == "DISPLACEMENT_LOW") {
		field.type = InstructionFieldTypes::DISPLACEMENT_LOW;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 8;
	}
	if (split_data[0] == "DISPLACEMENT_HIGH") {
		field.type = InstructionFieldTypes::DISPLACEMENT_HIGH;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 8;
	}
	if (split_data[0] == "DATA") {
		field.type = InstructionFieldTypes::DATA;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 8;
	}
	if (split_data[0] == "DATA_EXTENDED") {
		field.type = InstructionFieldTypes::DATA_EXTENDED;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 8;
	}
	if (split_data[0] == "DATA_IF_W") {
		field.type = InstructionFieldTypes::DATA_IF_W;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 8;
	}
	if (split_data[0] == "DATA_IF_SW") {
		field.type = InstructionFieldTypes::DATA_IF_SW;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 8;
	}
	if (split_data[0] == "ADDRESS_LOW") {
		field.type = InstructionFieldTypes::ADDRESS_LOW;
		assert(split_data.size() == 1);
		
		field.length_in_bits = 8;
	}
	if (split_data[0] == "ADDRESS_HIGH") {
		field.type = InstructionFieldTypes::ADDRESS_HIGH;
		assert(split_data.size() == 1);	
		field.length_in_bits = 8;
	}
	if (split_data[0] == "DATA_8") {
		field.type = InstructionFieldTypes::DATA_8;
		assert(split_data.size() == 1);	
		field.length_in_bits = 8;
	}
	
	if (field.type == InstructionFieldTypes::UNKNOWN) {
		assert(false);
	}

	return field;
}


std::vector < SingleInstructionSpecification > parse_instruction_specification(std::string& input_data) {
	const std::string INSTRUCTION_START_STRING = "<";
	const std::string INSTRUCTION_END_STRING = ">";

	std::stringstream data(input_data);
	
	std::vector<SingleInstructionSpecification> instruction_specification; 
	SingleInstructionSpecification current_instruction_specification;

	bool is_parsing_instruction = false;
	for (std::string line; std::getline(data, line);) {
		if (line.starts_with(INSTRUCTION_START_STRING)) {
			assert(!is_parsing_instruction);
			// can only start an instruction if we are not already inside one
			is_parsing_instruction = true;

			// reset the current instruction specification.
			current_instruction_specification = {};

			
			// parse the mnemonic and debug_name 
			std::string rest_of_line = line.substr(INSTRUCTION_END_STRING.size(), line.size());
			std::vector<std::string> split = split_string_by_delimiter_and_remove_any_whitespaces(rest_of_line, ',');
			assert(split.size() == 2);
			current_instruction_specification.mnemonic = split[0];
			current_instruction_specification.debug_name = split[1];
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


std::string read_entire_file(const std::string& filename) {
	std::ifstream t(filename);
	std::stringstream file_contents_stream;
	file_contents_stream << t.rdbuf();

	return file_contents_stream.str();
}