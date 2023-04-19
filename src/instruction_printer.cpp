#include "instruction_printer.hpp"

#include <cassert> 


std::string to_string(const Instruction& instruction) {
	return "[Instruction | address: " + std::to_string(instruction.address) + ", mnemonic: " + instruction.mnemonic + ", source: " + instruction.source.repr + ", destination: " + instruction.destination.repr + ", is_wide: " + std::to_string(instruction.is_wide) + "]";
}

std::string to_string(const InstructionField& field) {
	return "<field | field.type: " + to_string(field.type) + ", field.length_in_bits: " + std::to_string(field.length_in_bits) + ", field.binary_string: " + std::to_string(field.binary_string) +
		", field.value: " + std::to_string(field.value) + ">";
}

std::string to_string(const SingleInstructionSpecification& spec) {
	std::string repr = "mnemonic: " + spec.mnemonic + "\ndebug_name: " 
		+ spec.debug_name + "\n";
	for (const InstructionField& field: spec.instruction_fields) {
		repr += to_string(field) + "\n";
	}
	return repr;
}

std::string to_string(const InstructionFieldTypes type) {
	std::string repr = "";
	switch (type) {
		case UNKNOWN:
			repr = "UNKNOWN";
			break;
		case LITERAL:
			repr = "LITERAL";
			break;
		case MOD:
			repr = "MOD";
			break;
		case REG:
			repr = "REG";
			break;
		case RM:
			repr = "RM";
			break;
		case SR:
			repr = "SR";
			break;
		case D_BIT:
			repr = "D_BIT";
			break;
		case W_BIT:
			repr = "W_BIT";
			break;
		case S_BIT:
			repr = "S_BIT";
			break;
		case V_BIT:
			repr = "V_BIT";
			break;
		case DISPLACEMENT_LOW:
			repr = "DISPLACEMENT_LOW";
			break;
		case DISPLACEMENT_HIGH:
			repr = "DISPLACEMENT_HIGH";
			break;
		case DATA:
			repr = "DATA";
			break;
		case DATA_EXTENDED:
			repr = "DATA_EXTENDED";
			break;
		case DATA_IF_W:
			repr = "DATA_IF_W";
			break;
		case DATA_IF_SW:
			repr = "DATA_IF_SW";
			break;
		case ADDRESS_LOW:
			repr = "ADDRESS_LOW";
			break;
		case ADDRESS_HIGH:
			repr = "ADDRESS_HIGH";
			break;
		case DATA_8:
			repr = "DATA_8";
			break;
		default:
			// if this happens, something elsewhere has gone bad. might change in the future
			// at the moment this is a reminder to not forget to implement a new enum case
			assert(false);
			break;
	}

	return repr;
}