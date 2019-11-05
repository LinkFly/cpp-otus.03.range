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
bool ip_checker(const Ip& ip_parts) {
	int req_parts[] = { (targs)... };
	for (int i = 0; i < sizeof...(targs); i++) {
		if (req_parts[i] != ip_parts[i])
			return false;
	}
	return true;
}

auto defAnyChecker(int num) {
	return [num_ = num](const Ip& ip_parts) {
		auto fn_eq_num = [num__ = num_](auto part) {return part == num__; };
		return std::any_of(ip_parts.begin(), ip_parts.end(), fn_eq_num);
	};
}

template<typename T>
void PoolCollection<T>::add_from_line(ip_pool<T>& ip_pool, std::string& line) {
	std::vector<std::string> v = split(line, '\t');
	std::vector<std::string> snums = split(v.at(0), '.');
	Ip ip_parts;
	for (int i = 0; i < 4; i++) {
		using ip_part_type = typename std::remove_reference<decltype(ip_parts[i])>::type;
		ip_parts[i] = static_cast<ip_part_type>(stoi(snums[i]));
	}
	ip_pool.push_back(ip_parts);
}

template<class T>
void PoolCollection<T>::base_sort(ip_pool<T>* ip_pool) {
	if (ip_pool == nullptr) ip_pool = &base_pool;
	ip_pool->sort(std::greater<T>());
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
		ostr << (int)*it;
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

	// Output sorted base_pool
	output_pools(out, std::vector<ip_pool_ptr>{ &pool });

	// Output ip which started 1
	auto itFirst1 = std::find_if_not(std::reverse_iterator<ip_pool_iterator>(pool.rbegin()), pool.rend(), ip_checker<1>).base();
	fnOutputIpRange(itFirst1, pool.end());

	auto fnLowerBound = [](T left, int value) {return left[0] > value; };
	auto fnLowerBoundIdx1 = [](T left, int value) {return left[1] > value; };
	auto fnUppperBound = [](int value, auto right) {return value > right[0]; };

	auto itFinded46 = lower_bound(pool.begin(), pool.end(), 46, fnLowerBound);
	auto itFindedNot46 = upper_bound(itFinded46, pool.end(), 46, fnUppperBound);
	auto itFindedSecond70 = lower_bound(itFinded46, itFindedNot46, 70, fnLowerBoundIdx1);

	fnOutputWhile(itFindedSecond70, itFindedNot46, [](const T& ip_parts) {return ip_parts[1] == 70; });

	// Output ip which includes 46
	fnOutputWhile(pool.begin(), pool.end(), defAnyChecker(46));
}

template<typename T>
void PoolCollection<T>::read_to_pool(std::istream& in, ip_pool<T>& ip_pool) {
	read_lines(in, [&ip_pool](std::string line) {
		add_from_line(ip_pool, line);
		});
}

void run(std::istream &in, std::ostream &out) {
	using PoolCol = PoolCollection<Ip>;
	PoolCol ip_pools_col;

	ip_pools_col.read_to_pool(in, ip_pools_col.base_pool);

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
