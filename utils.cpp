#include <vector>
#include <iterator>
#include <iostream>
#include <functional>
#include <string>

#include "utils.h"

void read_lines(std::istream& stream, func_str fn_line_handler) {
	for (std::string line; std::getline(stream, line);)
		fn_line_handler(line);
}
