#include <string>
#include <fstream>
#include <sstream>

#include "io_utils.hpp"

std::string read_entire_file(const std::string& filename) {
	std::ifstream t(filename);
	std::stringstream file_contents_stream;
	file_contents_stream << t.rdbuf();

	return file_contents_stream.str();
}

// @Cleanup: bad way to print a byte for stdio, but its just for debugging
std::string char_to_binary_string(char c) {
	std::string result = "";

	char c0 = c & 0b00000001;
	char c1 = c & 0b00000010;
	char c2 = c & 0b00000100;
	char c3 = c & 0b00001000;
	char c4 = c & 0b00010000;
	char c5 = c & 0b00100000;
	char c6 = c & 0b01000000;
	char c7 = c & 0b10000000;

	if (c7) {
		result += "1";
	} else {
		result += "0";
	}
	if (c6) {
		result += "1";
	} else {
		result += "0";
	}
	if (c5) {
		result += "1";
	} else {
		result += "0";
	}
	if (c4) {
		result += "1";
	} else {
		result += "0";
	}
	if (c3) {
		result += "1";
	} else {
		result += "0";
	}
	if (c2) {
		result += "1";
	} else {
		result += "0";
	}
	if (c1) {
		result += "1";
	} else {
		result += "0";
	}
	if (c0) {
		result += "1";
	} else {
		result += "0";
	}

	return result;
}