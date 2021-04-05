#include "parser.h"

void generic_parser(const std::string& filepath, const parser_map& parsers) {
	return generic_parser(filepath, parsers, [](const std::string& instruction, std::istringstream& line) {
		throw std::runtime_error("Unknown instruction type '" + instruction + "'");
	});
}

void generic_parser(const std::string& filepath, const parser_map& parsers, const parser_function& unknown) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open level file");
	}

	// Parse line-by-line
	std::string lineText;
	while (file.good() && std::getline(file, lineText)) {
		std::istringstream line(lineText);
		std::string instruction;
		// Check for reasons to skip the line
		if (
			!(line >> instruction) || // line is empty
			(instruction[0] == '#') // line is comment (starts with #)
		) {
			continue;
		}

		auto it = parsers.find(instruction);
		auto& parser = it != parsers.end() ? it->second : unknown;
		parser(instruction, line);
	}
}
