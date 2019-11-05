#pragma once

#include <iostream>
#include <array>
#include <map>
#include <algorithm>

#include "build/config.h"
#include "utils.h"

//// Types

using Byte = unsigned char;
using Ip = std::array<Byte, 4>;

template<typename T, class Iter, class RevIter>
class iip_pool {
private:
	virtual Iter begin() = 0;
	virtual Iter end() = 0;
	virtual RevIter rbegin() = 0;
	virtual RevIter rend() = 0;

	virtual void push_back(const T& val) = 0;
	/*virtual void reserve(size_t size) = 0;*/
	virtual T& operator[](const size_t& idx) = 0;
	virtual size_t size() = 0;
	virtual void sort(std::function<bool(const T & left, const T & right)> predicat) = 0;
};

template<class T>
class ip_pool_vector: public iip_pool<T, typename std::vector<T>::iterator, typename std::vector<T>::reverse_iterator> {
	std::vector<T> pool;
public:
	using iterator = typename std::vector<T>::iterator; // try pool
	using reverse_iterator = typename std::vector<T>::reverse_iterator; // try pool
	/*using container_type = std::vector<T>;*/
	iterator begin() {
		return pool.begin();
	};
	iterator end()  {
		return pool.end();
	}
	reverse_iterator rbegin() {
		return pool.rbegin();
	}
	reverse_iterator rend() {
		return pool.rend();
	}

	void push_back(const T& val) {
		pool.push_back(val);
	}
	/*void reserve(size_t size) {
		pool.reserve(size);
	}*/
	T& operator[](const size_t& idx) {
		return pool[idx];
	}
	size_t size() {
		return pool.size();
	}
	void sort(std::function<bool(const T & left, const T & right)> predicat) {
		std::sort(begin(), end(), predicat);
	}
};

template<class T>
class ip_pool_arlist : public iip_pool<T, typename std::list<T>::iterator, typename std::list<T>::reverse_iterator> {
	std::list<T> pool;
public:
	using iterator = typename std::list<T>::iterator; // try pool
	using reverse_iterator = typename std::list<T>::reverse_iterator;
	/*using container_type = std::list<T>;*/
	iterator begin() {
		return pool.begin();
	};
	iterator end() {
		return pool.end();
	}
	reverse_iterator rbegin() {
		return pool.rbegin();
	}
	reverse_iterator rend() {
		return pool.rend();
	}

	void push_back(const T& val) {
		pool.push_back(val);
	}
	//void reserve(size_t size) {
	//	pool.reserve(size);
	//}
	T& operator[](const size_t& idx) {
		auto cur_idx = 0;
		for (auto& cur : pool) {
			if (cur_idx++ == idx)
				return cur;
		}
		throw "Bad index";
	}
	size_t size() {
		return pool.size();
	}
	void sort(std::function<bool(const T & left, const T & right)> predicat) {
		pool.sort(predicat);
	}
};

#ifdef USE_VECTOR
	template<class T>
	using ip_pool = ip_pool_vector<T>;
#endif

#ifdef USE_LIST
	template<class T>
	using ip_pool = ip_pool_arlist<T>;
#endif

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
//////////////////////////////////////

template<class T>
struct PoolCollection {
	using ip_pool_ptr = ip_pool<T>*;
	ip_pool<T> base_pool, ip_pool_started_1, ip_pool_started_46_70, ip_pool_includes_46;
	static void add_from_line(ip_pool<T>& ip_pool, std::string& line);
	static void read_to_pool(std::istream& in, ip_pool<T>& ip_pool);
	void base_sort(ip_pool<T>* ip_pool = nullptr);
	void classify();
	std::vector<ip_pool_ptr> get();
	std::string unpack_ip(const T& ip_parts);
	void output_pools(std::ostream& out, const std::vector<ip_pool_ptr>& pools);
	void filtering_and_output_pools(std::ostream& out);
};

void run(std::istream& in = std::cin, std::ostream& out = std::cout);
