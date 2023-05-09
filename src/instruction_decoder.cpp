#include "instruction_decoder.hpp"

#include "io_utils.hpp"
#include "instruction_printer.hpp"
#include <cassert>
#include <iostream>


InstructionInfo fill_out_next_instruction_info(DecodingContext& decoding_context) {
	InstructionInfo info = {};

	std::vector<uint8_t> data = decoding_context.data;
	if (decoding_context.address >= data.size()) {
		info.is_valid = false;
		return info;
	}

	// @Speed: We are currently going through EVERY instruction specification and 
	// checking if it matches the current byte stream starting at address.
	// This is obviously super slow, but we deliberately do this, since we will later
	// speed this up via a decision tree like structure. The point is that, we can and 
	// improve this drastically but for now we get the benefit of a great deal of
	// flexibility instead of manually trying to find matching instruction types to 
	// find fast decoding paths. 
	for (const SingleInstructionSpecification& instruction_spec: decoding_context.specification) {
		info = try_to_find_matching_instruction_specification(decoding_context, instruction_spec);
		if (info.is_valid) {
			// The raw data stream starting at address matches the current instruction 
			// specification.
			info.matching_specification = instruction_spec;
			return info;
		}
	}

	//
	std::cout << "\n\n\nERROR: The current byte stream could not be matched with any instruction specification.\n\n\n";
	return info;
}

InstructionInfo try_to_find_matching_instruction_specification(DecodingContext& decoding_context, const SingleInstructionSpecification& instruction_spec) {
	InstructionInfo info = {};
	info.address = decoding_context.address;

	std::vector<uint8_t> data = decoding_context.data;
	unsigned int current_position = decoding_context.address; // position in bytes in the raw data stream.

	uint8_t current_byte = data[current_position];
	info.raw_data.push_back(current_byte);
	// always start at the first bit in the current byte, there is no straddling between bytes (remember that its from highest to lowest bit, that is bit 8 (highest) is at position_in_current_byte_in_bits equal to 0).
	int position_in_current_byte_in_bits = 0;

	std::cout << "Try to match stream to instruction specification.\n"; 
	std::cout << "spec.debug_name: " << instruction_spec.debug_name << "\n";

	bool has_read_the_mod_field = false; // sanity check variable, can be removed in a cleanup. Is there to make sure we have set the mod field before accessing it to determine any possible displacement that might occur.
	bool has_read_the_w_field = false;
	bool has_read_the_rm_field = false;
	bool reached_end_of_stream_before = false;

	int displacement_length = 0; // in bytes
	for (const InstructionField& field: instruction_spec.instruction_fields) {
		// @Cleanup: sanity checks, will get removed
		has_read_the_mod_field |= (field.type == InstructionFieldTypes::MOD);
		has_read_the_w_field |= (field.type == InstructionFieldTypes::W_BIT);
		has_read_the_rm_field |= (field.type == InstructionFieldTypes::RM);

		// @Cleanup: factor this out into a method:
		// auto flags = set_flags_from_implicit_fields(field, info);
		// where we set flags depending on the fields, for example if we get to the 
		// RM field we should determien (with the value of the MOD field) what the 
		// displacement length is and set flags indicating if there is displacement, 
		// if its wide or not etc, same for the DATA_IF_W field based on the W_BIT field.
		// Below we can then handle these implicit fields based on the set flags and only
		// put data into the fields if the flag is set. For now we do it kinda messy here.
		if (has_read_the_mod_field && has_read_the_rm_field) {
			uint8_t mod_field_value = info.fields_data[InstructionFieldTypes::MOD];
			uint8_t rm_field_value = info.fields_data[InstructionFieldTypes::RM];
			switch (static_cast<int>(mod_field_value)) {
				case 0b00:
					displacement_length = 0;
					if (rm_field_value == 0b110) {
						displacement_length = 2;
					}
					break;
				case 0b01:
					displacement_length = 1;
					break;
				case 0b10:
					displacement_length = 2;
					break;
				case 0b11:
					displacement_length = 0;
					break;
				default:
					// should never reach this, the mod field is just 2 bits wide.
					assert(false);
					break;
			}
		}
		
		// In the Intel 8086 specification, the displacement fields are implicit,
		// that means their existence depends on the MOD and RM field. 
		assert(displacement_length >= 0 && displacement_length <= 2);
		if (field.type == InstructionFieldTypes::DISPLACEMENT_LOW) {
			if (displacement_length == 0) {
				continue;
			}
		} else if (field.type == InstructionFieldTypes::DISPLACEMENT_HIGH) {
			if (displacement_length != 2) {
				continue;
			}
		} else if (field.type == InstructionFieldTypes::DATA_IF_W) {
			uint8_t w_bit_value = info.fields_data[InstructionFieldTypes::W_BIT];
			// only read the DATA_IF_W field if the w_bit is set.
			if (!w_bit_value) {
				continue; 
			}
		}
		
		// we have to handle all implicit data fields above, and determine if they occur or not, ie if we have to read data into them or not.
		if (reached_end_of_stream_before) {
			// The following check is there to handle the case that we reached the end of the input data stream and still are not at the end of the specification. It could be the case that the rest of the specification are implicit data fields, which are going to be skipped. So we do all of these implicit skip checks above, and hence, if we reach this line and have reached the end of the stream in some previous iteration we know that this specification doesnt match the byte stream and we return
			info.is_valid = false;
			return info;
		}


		unsigned int how_many_bits_to_read = field.length_in_bits;

		// just check if we have some corrupted instruction specifications
		assert(!(field.type == InstructionFieldTypes::UNKNOWN || field.type == InstructionFieldTypes::COUNT));

		// extract the data for the current field from the current byte.
		uint8_t extracted_field_bits = read_bits_in_highest_to_lowest_order_from_byte(current_byte, position_in_current_byte_in_bits, how_many_bits_to_read);

		// advance our read position in the current byte
		position_in_current_byte_in_bits += how_many_bits_to_read; 
		assert(position_in_current_byte_in_bits <= 8);

		if (field.type == InstructionFieldTypes::LITERAL) {
			// if its a literal we additionally have to check that the extracted bits are equal to our expected value, if they dont match the 
			// current instruction specification doesnt match this raw byte sequence and we exit out.
			if (extracted_field_bits != field.value) {
				info.is_valid = false;
				return info;
			}
		}

		// assert that we only set a field, that is not a literal if we havent already set it before.
		assert(field.type == InstructionFieldTypes::LITERAL || info.already_set_fields_array[field.type] == false);

		info.fields_data[field.type] = extracted_field_bits;
		info.already_set_fields_array[field.type] = true;

		if (position_in_current_byte_in_bits == 8) { 
			info.offset_in_bytes++;
			// we are at the end of the current byte, have to read the next byte.
			if (current_position + 1 >= data.size()) {
				// we reached the end of the stream, hence we cant read the next byte. But we also cant just say that the spec doesnt match here, since we might just be at the end of the spec (the easy case) or that we have only implicit fields left (like displacement fields). In the latter case we have to check that the mod-field actually says that the implicit displacement fields dont occur, in order to see if the current specification matches. 
				// we do this check by setting the following status variable and going into the next iteration and checking that all implicit fields do not occur. 
				reached_end_of_stream_before = true;
				continue;
			}
			// can read the next byte
			current_position++; // advance position BEFORE reading the next byte.
			current_byte = data[current_position];
			position_in_current_byte_in_bits = 0; // reset current bit position to 0
			info.raw_data.push_back(current_byte);
		}
	}

	// could match the byte sequence to the specification
	info.is_valid = true;

	std::cout << "####################################################\n";
	std::cout << "Matched byte stream to instruction specification.\n";
	std::cout << "The byte stream was:\n";
	for (int i = 0; i < info.offset_in_bytes; i++) {
		std::cout << char_to_binary_string(decoding_context.data[decoding_context.address + i]) << " ";
	}
	std::cout << "\n";
	std::cout << "The matching specification was:\n";
	std::cout << to_string(instruction_spec);
	return info;
}

std::vector<Instruction> decode_instruction_stream(DecodingContext& decoding_context) {
	// @Cleanup @Memory: Do a better guess for the amount to reserve? 
	// We assume each instruction is 1 byte long to not undershoot, 
	// but this is probably a complete overshoot.
	std::vector<Instruction> decoded_instructions = {};
	decoded_instructions.reserve(decoding_context.data.size());


	uint32_t address_before = decoding_context.address;
	while(decoding_context.address < decoding_context.data.size()) {
		Instruction current_instruction = decode_next_instruction_and_update_context(decoding_context);

		
		std::cout << "Decoded instruction:\n" << to_string(current_instruction) << "\n";
		decoded_instructions.push_back(current_instruction);
	}


	return decoded_instructions;
}

Instruction decode_next_instruction_and_update_context(DecodingContext& decoding_context) {
	Instruction instruction = {};

	InstructionInfo info = fill_out_next_instruction_info(decoding_context);

	assert(info.is_valid);
	instruction.address = decoding_context.address;
	instruction.mnemonic = info.matching_specification.mnemonic;

	// advance to next address
	decoding_context.address += info.offset_in_bytes;
	return instruction;
}


std::string lookup_register_name(bool is_wide, uint8_t lookup_value) {
	std::string register_name = "UNKNOWN_REGISTER_NAME";
	if (!is_wide) {
		switch (lookup_value) {
			case 0b000:
				register_name = "AL";
				break;
			case 0b001:
				register_name = "CL";
				break;
			case 0b010:
				register_name = "DL";
				break;
			case 0b011:
				register_name = "BL";
				break;
			case 0b100:
				register_name = "AH";
				break;
			case 0b101:
				register_name = "CH";
				break;
			case 0b110:
				register_name = "DH";
				break;
			case 0b111:
				register_name = "BH";
				break;
			default:
				// The lookup_value should never have any other value, except those above.
				assert(false);
				break;
		}
	} else {
		switch (lookup_value) {
			case 0b000:
				register_name = "AX";
				break;
			case 0b001:
				register_name = "CX";
				break;
			case 0b010:
				register_name = "DX";
				break;
			case 0b011:
				register_name = "BX";
				break;
			case 0b100:
				register_name = "SP";
				break;
			case 0b101:
				register_name = "BP";
				break;
			case 0b110:
				register_name = "SI";
				break;
			case 0b111:
				register_name = "DI";
				break;
			default:
				// The lookup_value should never have any other value, except those above.
				assert(false);
				break;
		}
	}
	return register_name;
}

uint8_t assert_field_has_been_set_and_get_value(const InstructionInfo& info, InstructionFieldTypes field_type) {
	assert(info.already_set_fields_array[field_type]);

	return info.fields_data[field_type];
}


Instruction convert_info_to_instruction(const InstructionInfo& info) {
	assert(info.is_valid);

	Instruction instruction = {};
	instruction.address = info.address;

	if (info.already_set_fields_array[InstructionFieldTypes::W_BIT]) {
		// W_BIT field has been set.
		instruction.is_wide = info.fields_data[InstructionFieldTypes::W_BIT] == 1;
	}

	std::string src_operand_register_name = lookup_register_name(instruction.is_wide, info.fields_data[InstructionFieldTypes::REG]);
	instruction.source_operand.repr = src_operand_register_name;

	return instruction;
}


std::string determine_effective_address(const InstructionInfo& info) {
	std::string result = "";

	uint8_t mod_field_value = assert_field_has_been_set_and_get_value(info, InstructionFieldTypes::MOD);
	uint8_t rm_field_value = assert_field_has_been_set_and_get_value(info, InstructionFieldTypes::RM);
	bool is_direct_addressing_mode = (mod_field_value == 0b00) && (rm_field_value == 0b110);

	if (mod_field_value == 0b11) {
		// Register mode.
		uint8_t w_field_value = assert_field_has_been_set_and_get_value(info, InstructionFieldTypes::W_BIT);
		bool is_wide = w_field_value == 1;
			
		// use the rm_field value to lookup the register name for the effective address calculation.
		result = lookup_register_name(rm_field_value, is_wide);
	} else {
		// Memory mode.
		switch (rm_field_value) {
			case 0b000:
				result = "[BX + SI";
				break;
			case 0b001:
				result = "[BX + DI";
				break;
			case 0b010:
				result = "[BP + SI";
				break;
			case 0b011:
				result = "[BP + DI";
				break;
			case 0b100:
				result = "[SI";
				break;
			case 0b101:
				result = "[DI";
				break;
			case 0b110:
				if (is_direct_addressing_mode) {
					result = "[";
				} else {
					result = "[BP";
				}
				break;
			case 0b111:
				result = "[BX";
				break;
			default:
				// invalid rm_field value.
				assert(false);
				break;
		}

		int displacement_length = 0; // number of displacement bytes, either 0, 1 or 2, corresponding to no DISPLACEMENT_LOW and DISPLACEMENT_HIGH field, or just the DISPLACEMENT_LOW field, or both fields, respectively.

		switch (mod_field_value) {
			case 0b00:
				if (is_direct_addressing_mode) {
					displacement_length = 2;
				} else {
					displacement_length = 0;
				}
				break;
			case 0b01:
				displacement_length = 1;
				break;
			case 0b10:
				displacement_length = 2;
				break;
			case 0b11:
				// cant happen, is handled above.
				assert(false);
				break;
			default:
				// Invalid mod_field_value.
				assert(false);
				break;
		}


		uint16_t displacement_value = 0;
		if (displacement_length == 1) {
			displacement_value = assert_field_has_been_set_and_get_value(info, InstructionFieldTypes::DISPLACEMENT_LOW); // @Fixme. these are unsigned types? no sign extension here, but we want one?
		} else if (displacement_length == 2) {
			uint8_t displacement_low_value = assert_field_has_been_set_and_get_value(info, InstructionFieldTypes::DISPLACEMENT_LOW);
			uint8_t displacement_high_value = assert_field_has_been_set_and_get_value(info, InstructionFieldTypes::DISPLACEMENT_HIGH);

			displacement_value = (displacement_high_value << 8) | displacement_low_value;
		}

		if (displacement_length > 0) {
			result += " " + std::to_string(displacement_value);
		}
		result += "]";

	}

	return result; 
}



// reads bits from position, position + 1, position + 2, ..., position + number_of_bits_to_read - 1
// reads highest to lowest bit order, ie position = 0 means start at highest bit of the given byte.
uint8_t read_bits_in_highest_to_lowest_order_from_byte(uint8_t byte, uint8_t position, uint8_t number_of_bits_to_read) {
	assert(position <= 7);
	assert(position + number_of_bits_to_read <= 8);

	uint8_t mask = 0x00;

	// @Robustness: is this "dangerous"? using an unsigned type in a for loop, maybe the compiler will interpret the "<" check away under certain conditions? what im remembering is the case "x >= 0", where x is unsigned and gets decremented, which results in a condition that is always true hence the compiler uses that information -> unexpeted result. anyway i do not know, revisit this!
	for (uint8_t i = 0; i < number_of_bits_to_read; i++) {
		mask = (mask << 1) | 1;
	}

	int8_t num_bits_to_shift_left = 8 - static_cast<int8_t>(position) - static_cast<int8_t>(number_of_bits_to_read);
	mask = (mask << num_bits_to_shift_left);
	//std::cout << "mask: " << char_to_binary_string(mask) << "\n"; //nocheckin

	uint8_t extracted_bits = byte & mask;

	int8_t end_position = position + number_of_bits_to_read;
	int8_t down_shift_value = 8 - end_position;

	return extracted_bits >> down_shift_value;
}