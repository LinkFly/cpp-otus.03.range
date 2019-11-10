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
#include <range/v3/all.hpp>
//#include <string_view>

//for debug
//#include <typeinfo>

#include "build/config.h"
#include "ip_filter_lib.h"

using std::cout;
using std::endl;
using std::string;
using namespace std::string_literals;
//using namespace std::literals;
//using namespace std::placeholders;

namespace rv = ranges::v3::view;
//namespace rs = ranges::v3;
//namespace ra = ranges::v3::action;

template<int ...targs>
bool ip_checker(const Ip& ip_parts) {
	int req_parts[] = { (targs)... };
	for (int i = 0; i < sizeof...(targs); i++) {
		if (req_parts[i] != ip_parts[i])
			return false;
	}
	return true;
}

template<int ip_part>
bool any_checker(const Ip& ip_parts) {
	return std::any_of(ip_parts.begin(), ip_parts.end(),
		[](auto part) {return part == ip_part; });
}

template<typename T>
void PoolCollection<T>::add_from_line(ip_pool<T>& ip_pool, std::string& line) {
	std::vector<std::string> v = split(line, '\t');
	std::vector<std::string> snums = split(v.at(0), '.');
	Ip ip_parts;
	using ip_part_type = typename std::remove_reference< decltype(ip_parts[0]) >::type;
	for (int i = 0; i < 4; i++) {
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
		if (any_checker<46>(ip_parts))
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
		for (const auto& ip : *cur_pool | rv::transform(&unpack_ip)) {
			out << ip << "\n";
		}
	}
}

struct IpMask {
	Ip lower;
	Ip upper;
};

// Return struct for using into getBoundsByIpMask
IpMask parseIpMask(std::string mask) {
	IpMask res;
	std::vector<std::string> ip_parts = split(mask, '.');
	auto idx = 0;
	for (auto& ip_part : ip_parts) {
		if (ip_part == "*") {
			res.lower[idx] = 255;
			res.upper[idx] = 0;
		}
		else {
			auto ipart = stoi(ip_part);
			res.lower[idx] = ipart;
			res.upper[idx] = ipart;
		}
		++idx;
	}
	return res;
}

template<typename Iter>
struct Bounds {
	Iter lower;
	Iter upper;
};

// Return lower&upper iterators with finded bounds included all ip's match sIpMask
/** Example:
	getBoundsByIpMask(pool.begin(), pool.end(), "46.70.*.*"s)
*/
template<class Iter>
Bounds<Iter> getBoundsByIpMask(Iter itStart, Iter itEnd, const std::string& sIpMask) {
	IpMask ipMask = parseIpMask(sIpMask);
	Bounds<Iter> res;
	res.lower = lower_bound(itStart, itEnd, ipMask.lower, std::greater{});
	res.upper = upper_bound(res.lower, itEnd, ipMask.upper, std::greater{});
	return res;
}

// Return lambda which unpack and output ip
template<typename T>
auto PoolCollection<T>::getFnIpOutput(std::ostream& out) {
	return [&out](auto& cur_ip) {
		std::string sIP = PoolCollection<T>::unpack_ip(cur_ip);
		out << sIP << endl;
	};
}

/** Example:
filtering_and_output_by_mask(out, pool, "46.70.*.*"s);
*/
template<typename T>
void PoolCollection<T>::filtering_and_output_by_mask(
	std::ostream& out, ip_pool<T>& pool, const string& ip_mask)
{
	auto fnIpOutput = PoolCollection<T>::getFnIpOutput(out);
	Bounds<typename ip_pool<T>::iterator> bounds = getBoundsByIpMask<typename ip_pool<T>::iterator>(
		pool.begin(), pool.end(), ip_mask);
	std::for_each(bounds.lower, bounds.upper, fnIpOutput);
}

// TODO! (Analize it) Don't call unpack_ip (make special structure)
template<typename T>
void PoolCollection<T>::filtering_and_output_pools(std::ostream& out) {
	using ip_pool_iterator = typename ip_pool<T>::iterator;
	ip_pool<T>& pool = base_pool;

	// Helpers
	auto fnIpOutput = PoolCollection<T>::getFnIpOutput(out);

	// Output sorted base_pool
	output_pools(out, std::vector<ip_pool_ptr>{ &pool });

	// Output ip which started 1
	for (const auto& ip : pool | rv::reverse | rv::filter(ip_checker<1>) | rv::reverse) {
		fnIpOutput(ip);
	}

	// Output ip which started 47.70.
	filtering_and_output_by_mask(out, pool, "46.70.*.*"s);

	// Output ip which includes 46
	for (const auto& ip: pool | rv::filter(any_checker<46>))
		fnIpOutput(ip);
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
