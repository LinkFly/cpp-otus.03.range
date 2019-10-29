#pragma once

#include <vector>
#include <functional>

//// Types
using vecstr = std::vector<std::string>;
using vecint = std::vector<int>;
using vec_vecint = std::vector<vecint>;
using func_str = std::function<void(std::string)>;

//// Utils
std::vector<std::string> split(const std::string& str, char d);

void read_lines(std::istream& stream, func_str fn_line_handler);
