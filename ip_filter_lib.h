#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>
//#include <map>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string& str, char d);

using vecstr = std::vector<std::string>;
using vec_vecstr = std::vector<vecstr>;
using func_str = std::function<void(std::string)>;

bool check_started_1(const std::vector<int>& ip_parts);

bool check_started_46_70(const std::vector<int>& ip_parts);

bool check_includes_46(const std::vector<int>& ip_parts);

void read_lines(std::istream& stream, func_str fn_line_handler);

template<class T>
void process_vector(std::vector<T> vec, std::function<void(T)> fn);

std::string unpack_ip(vecstr& ip_parts);

void add_line_to_pool(vec_vecstr& ip_pool, std::string line);

vecstr read_file(std::string filename);

std::string lines_str(vecstr vec);

template<class T>
using ip_pool = std::vector<T>;

template<class T>
void output_pools(std::ostream& out, std::vector<ip_pool<T>> pools, std::function<std::string(T)> fn_prepare_ip) {
	for (auto cur_pool : pools) {
		for (T ip_desc : cur_pool) {
			std::string ip = fn_prepare_ip(ip_desc);
			out << ip << "\n";
		}

	}
}

template<class T = vecstr>
struct PoolCollection {
	ip_pool<T> base_pool, ip_pool_started_1, ip_pool_started_46_70, ip_pool_includes_46;
	void base_sort();
	void classify();
	std::vector<ip_pool<T>> get();
private:
	void pool_sort(ip_pool<vecstr>& ip_pool);
};

void run(std::istream& in, std::ostream& out);