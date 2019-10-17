#include <iostream>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>

#include "ip_filter_lib.h"

void run(std::istream& in, std::ostream& out);

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string& str, char d)
{
	std::vector<std::string> r;

	std::string::size_type start = 0;
	std::string::size_type stop = str.find_first_of(d);
	while (stop != std::string::npos)
	{
		r.push_back(str.substr(start, stop - start));

		start = stop + 1;
		stop = str.find_first_of(d, start);
	}

	r.push_back(str.substr(start));

	return r;
}

using vecstr = std::vector<std::string>;

using vec_vecstr = std::vector<vecstr>;
using func_str = std::function<void(std::string)>;

bool check_started_1(const std::vector<int>& ip_parts) {
	return ip_parts[0] == 1;
}

bool check_started_46_70(const std::vector<int>& ip_parts) {
	return ip_parts[0] == 46 && ip_parts[1] == 70;
}

bool check_includes_46(const std::vector<int>& ip_parts) {
	for (auto ip_part : ip_parts) {
		if (ip_part == 46)
			return true;
	}
	return false;
}

void read_lines(std::istream& stream, func_str fn_line_handler) {
	for (std::string line; std::getline(stream, line);)
		fn_line_handler(line);
}

template<class T>
void process_vector(std::vector<T> vec, std::function<void(T)> fn) {
	for (T val : vec) fn(val);
}

std::string unpack_ip(vecstr& ip_parts) {
	std::ostringstream ostr;
	for (auto it = ip_parts.cbegin(); it != ip_parts.cend(); it++) {
		if (it != ip_parts.cbegin()) {
			ostr << ".";
		}
		ostr << *it;
	}
	return ostr.str();
}

//template<class T>
//void process_vector(std::vector<T> vec, std::function<void(T)> fn) {

void add_line_to_pool(vec_vecstr& ip_pool, std::string line) {
	std::vector<std::string> v = split(line, '\t');
	ip_pool.push_back(split(v.at(0), '.'));
}

vecstr read_file(std::string filename) {
	vecstr vec;
	std::ifstream fin(filename, std::ios::in);
	read_lines(fin, [&vec](std::string line) {
		vec.push_back(line);
		});
	return vec;
}

std::string lines_str(vecstr vec) {
	std::ostringstream sout;
	for (auto s : vec) {
		sout << s << "\n";
	}
	return sout.str();
}

template<class T>
using ip_pool = std::vector<T>;

//void output_pools(std::ostringstream out,
template<class T>
void output_pools(std::ostream& out, std::vector<ip_pool<T>> pools, std::function<std::string(T)> fn_prepare_ip);

template<class T>
void PoolCollection<T>::base_sort() {
	pool_sort(base_pool);
}

template<class T>
void PoolCollection<T>::classify() {
	for (auto cur_ip : base_pool) {
		std::vector<int> ip_parts;
		for (auto s_ip_part : cur_ip) {
			ip_parts.push_back(std::stoi(s_ip_part));
		}
		if (check_started_46_70(ip_parts))
			ip_pool_started_46_70.push_back(cur_ip);
		if (check_includes_46(ip_parts))
			ip_pool_includes_46.push_back(cur_ip);
		if (check_started_1(ip_parts))
			ip_pool_started_1.push_back(cur_ip);
	}
}

template<class T>
std::vector<ip_pool<T>> PoolCollection<T>::get() {
	return { base_pool, ip_pool_started_1, ip_pool_started_46_70, ip_pool_includes_46 };
}

template<class T>
void PoolCollection<T>::pool_sort(ip_pool<vecstr>& ip_pool) {
	std::sort(ip_pool.begin(), ip_pool.end(), [](const vecstr& vs_left, const vecstr& vs_right) -> bool {
		for (int i = 0; i < 4; i++) {
			int num_left = std::stoi(vs_left[i]);
			int num_right = std::stoi(vs_right[i]);
			if (num_left != num_right) {
				return num_left > num_right;
			}
		}
		return false;
		});
}

void run(std::istream &in, std::ostream &out) {
	PoolCollection<vecstr> ip_pools_col;
	
	read_lines(in, [&ip_pool = ip_pools_col.base_pool](std::string line) {
		add_line_to_pool(ip_pool, line);
	});

	//  reverse lexicographically sort
	ip_pools_col.base_sort();

	// Prepare classified pools
	ip_pools_col.classify();

	//// Output
	
	auto ip_pools = ip_pools_col.get();
	output_pools<vecstr>(out, ip_pools, [](vecstr ip_desc) {
		return unpack_ip(ip_desc);
		});
}