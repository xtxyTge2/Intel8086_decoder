#pragma once

#include "instruction_specification_builder.hpp"

std::string to_string(InstructionFieldTypes type);
std::string to_string(InstructionField& field);
std::string to_string(SingleInstructionSpecification& spec);