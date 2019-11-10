#pragma once

#include <vector>
#include <functional>

//// Types

using func_str = std::function<void(std::string)>;

//// Utils

void read_lines(std::istream& stream, func_str fn_line_handler);
