#pragma once

#include "instruction_specification_builder.hpp"
#include "instruction_decoder.hpp"


std::string to_string(const Instruction& instruction);
std::string to_string(const InstructionFieldTypes type);
std::string to_string(const InstructionField& field);
std::string to_string(const SingleInstructionSpecification& spec);