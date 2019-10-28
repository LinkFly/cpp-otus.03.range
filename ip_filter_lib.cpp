#include <iostream>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <fstream>
#include <sstream>
//for debug
//#include <typeinfo>

#include "build/config.h"
#include "ip_filter_lib.h"

using std::cout;
using std::endl;

template<int ...targs>
bool ip_checker(const vecint& ip_parts) {
	int req_parts[] = { (targs)... };
	for (int i = 0; i < sizeof...(targs); i++) {
		if (req_parts[i] != ip_parts[i])
			return false;
	}
	return true;
}

auto defAnyChecker(int num) {
	return [num_ = num](const vecint& ip_parts) {
		auto fn_eq_num = [num__ = num_](auto part) {return part == num__; };
		return any_of(ip_parts.begin(), ip_parts.end(), fn_eq_num);
	};
}

template<typename T>
void PoolCollection<T>::add_from_line(ip_pool<T>& ip_pool, std::string& line) {
	vecstr v = split(line, '\t');
	vecstr snums = split(v.at(0), '.');
	vecint ip_parts;
	for (auto snum : snums) { ip_parts.push_back(stoi(snum)); }
	ip_pool.push_back(ip_parts);
}

template<class T>
void PoolCollection<T>::base_sort() {
	sort(base_pool.begin(), base_pool.end(), std::greater<T>());
}

template<class T>
void PoolCollection<T>::classify() {
	for (auto ip_parts : base_pool) {
		if (ip_checker<46, 70>(ip_parts))
			ip_pool_started_46_70.push_back(ip_parts);
		if (defAnyChecker(46)(ip_parts))
			ip_pool_includes_46.push_back(ip_parts);
		if (ip_checker<1>(ip_parts))
			ip_pool_started_1.push_back(ip_parts);
	}
}

template<class T>
std::vector<typename PoolCollection<T>::ip_pool_ptr> PoolCollection<T>::get() {
	return { &base_pool, &ip_pool_started_1, &ip_pool_started_46_70, &ip_pool_includes_46 };
}

template<typename T>
std::string PoolCollection<T>::unpack_ip(const T& ip_parts) {
	std::ostringstream ostr;
	for (auto it = ip_parts.cbegin(); it != ip_parts.cend(); it++) {
		if (it != ip_parts.cbegin()) {
			ostr << ".";
		}
		ostr << *it;
	}
	return ostr.str();
}

template<typename T>
void PoolCollection<T>::output_pools(std::ostream& out, const std::vector<PoolCollection::ip_pool_ptr>& pools) {
	for (auto *cur_pool : pools) {
		for (T ip_desc : *cur_pool) {
			std::string ip = unpack_ip(ip_desc);
			out << ip << "\n";
		}
	}
}

// TODO! Don't call unpack_ip (make special structure)
template<typename T>
void PoolCollection<T>::filtering_and_output_pools(std::ostream& out) {
	using ip_pool_iterator = typename ip_pool<T>::iterator;
	ip_pool<T>& pool = base_pool;

	// Helpers
	auto fnIpOutput = [this, &out](auto& cur_ip) {
		std::string sIP = this->unpack_ip(cur_ip);
		out << sIP << endl;
	};
	auto fnOutputIpRange = [&fnIpOutput](auto itFirst, auto itLast) {
		std::for_each(itFirst, itLast, fnIpOutput);
	};
	auto fnOutputWhile = [&fnIpOutput](auto itFirst, auto itLast, auto fnPredicat) {
		for (auto itCur = itFirst; itCur != itLast; itCur++) {
			auto& ip = *itCur;
			if (fnPredicat(ip)) {
				fnIpOutput(ip);
			}
		}
	};
	auto defExtractor = [](int part_idx) {
		return [idx=part_idx](const T& ip_cur) {return ip_cur[idx]; };
	};

	// Output sorted base_pool
	output_pools(out, std::vector<ip_pool_ptr>{ &pool });

	// Output ip which started 1
	auto itFirst1 = find_if_not(std::reverse_iterator<ip_pool_iterator>(pool.rbegin()), pool.rend(), ip_checker<1>).base();
	fnOutputIpRange(itFirst1, pool.end());

	auto itFinded46 = lower_bound_with_convertor(pool.begin(), pool.end(), 46, defExtractor(0));
	auto itFindedNot46 = lower_bound_with_convertor2(itFinded46, pool.end(), 46, defExtractor(0));
	auto itFindedSecond70 = lower_bound_with_convertor(itFinded46, itFindedNot46, 70, defExtractor(1));
	fnOutputWhile(itFindedSecond70, itFindedNot46, [](const T& ip_parts) {return ip_parts[1] == 70; });

	// Output ip which includes 46
	fnOutputWhile(pool.begin(), pool.end(), defAnyChecker(46));
}

void run(std::istream &in, std::ostream &out) {
	using PoolCol = PoolCollection<vecint>;
	PoolCol ip_pools_col;

	read_lines(in, [&ip_pools_col](std::string line) {
		PoolCol::add_from_line(ip_pools_col.base_pool, line);
	});

	//  reverse lexicographically sort
	ip_pools_col.base_sort();

	if (!ENABLED_OPTIMIZE_MEMORY) {
		// Prepare classified pools
		ip_pools_col.classify();
		//// Output
		ip_pools_col.output_pools(out, ip_pools_col.get());
	}
	else {
		ip_pools_col.filtering_and_output_pools(out);
	}
}
