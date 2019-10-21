#pragma once

// TODO! Remove not used includes
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>

//// Types
using vecstr = std::vector<std::string>;
using vecint = std::vector<int>;
using vec_vecint = std::vector<vecint>;
using func_str = std::function<void(std::string)>;

template<class T>
using ip_pool = std::vector<T>;

//////////////////////////////////////
//// Utils
void read_lines(std::istream& stream, func_str fn_line_handler);

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string& str, char d);

//// Predicats
bool check_started_1(const vecint& ip_parts);
bool check_started_46_70(const vecint& ip_parts);
bool check_includes_46(const vecint& ip_parts);

template<class T = vecstr>
struct PoolCollection {
	using ip_pool_ptr = ip_pool<T>*;
	ip_pool<T> base_pool, ip_pool_started_1, ip_pool_started_46_70, ip_pool_includes_46;
	static void add_from_line(ip_pool<T>& ip_pool, std::string& line);
	void base_sort();
	void classify();
	std::vector<ip_pool_ptr> get();
	std::string unpack_ip(const T& ip_parts);
	void PoolCollection<T>::output_pools(std::ostream& out, const std::vector<ip_pool_ptr>& pools);
	void filtering_and_output_pools(std::ostream& out);
private:
	void pool_sort(ip_pool<T>& ip_pool);
};

void run(std::istream& in = std::cin, std::ostream& out = std::cout);
