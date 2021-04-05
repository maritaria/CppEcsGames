#pragma once

#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

typedef std::function<void(const std::string& instruction, std::istringstream& line)> parser_function;
typedef std::map<std::string, parser_function> parser_map;

void generic_parser(const std::string& filepath, const parser_map& parsers);
void generic_parser(const std::string& filepath, const parser_map& parsers, const parser_function& unknown);
