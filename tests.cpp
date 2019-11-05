#define BOOST_TEST_MODULE ipfilter_test_module

#include <boost/test/unit_test.hpp>
#include <fstream>
//#include <filesystem>
#include <boost/filesystem.hpp>
#include <ctime>
#include <functional>
#include <algorithm>
#include <random>
#include <cmath>

#include "ip_filter_lib.h"
#include "utils.h"

////////////////////// Helpers ///////////////////////////////
using std::string;
struct Init {
	Init(std::function<void()> init_func) {
		init_func();
	}
};
#define INIT(init_func) struct Init init(init_func);

string read_file(string filename) {
	std::ifstream fin(filename, std::ios::in);
	string res;
	for (string line; std::getline(fin, line);)
		res += (line + "\n");
	return res;
}

using nested_evaluator = std::function< bool(const std::vector<int>&) >;
bool nested_loop(std::vector<int> counters, int max, nested_evaluator fn) {
	using func_loc = std::function<bool(int, std::vector<int>, int, nested_evaluator)>;
	func_loc _nested_loop_base = [&_nested_loop_base](int level, std::vector<int> counters, int max, nested_evaluator fn) {
		bool bRes = true;
		for (; counters[level] < max; ++(counters[level])) {
			if (level < counters.size() - 1)
				bRes = _nested_loop_base(level + 1, counters, max, fn);
			else
				bRes = fn(counters);
			if (!bRes)
				break;
		}
		return bRes;
	};
	return _nested_loop_base(0, counters, max, fn);
}
////////////////////// end Helpers ///////////////////////////////

#ifndef __PRETTY_FUNCTION__
#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#endif

const auto fixtures_in_filename = "ip_filter.tsv";
const auto fixtures_ot_filename = "ip_filter.tst";
const auto fixtures_ip_count = 1000;
//////////////////////////

#define ERR_NOT_FOUND 2

using std::cout;
using std::endl;
using std::string;

struct BaseFixtures {
	string str_in;
	string str_out;
} base_fixtures;

struct test_data {
	std::istringstream in;
	string* ptr_output_str;
};

std::tuple<std::string, std::string> get_fixtures_files() {
	/*std::filesystem::path cwd = std::filesystem::current_path();*/
	boost::filesystem::path cwd = boost::filesystem::current_path();
	auto test_files_dir = cwd / "..";

	auto file_in = (test_files_dir / "ip_filter.tsv").string();
	if (!boost::filesystem::exists(file_in)) {
		std::cerr << "not found file_in: " << file_in << std::endl;
		exit(ERR_NOT_FOUND);
	}
	auto file_test = (test_files_dir / "ip_filter.tst").string();
	if (!boost::filesystem::exists(file_test)) {
		std::cerr << "not found file_test: " << file_test << std::endl;
		exit(ERR_NOT_FOUND);
	}
	return std::tuple{ file_in, file_test };
}

// Before all tests
void init_base_fixtures() {
	auto [file_in, file_out] = get_fixtures_files();
	base_fixtures.str_in = read_file(file_in);
	base_fixtures.str_out = read_file(file_out);
}

// Before each test
test_data create_test_data() {
	test_data test_data;
	test_data.in = std::istringstream{ base_fixtures.str_in };
	test_data.ptr_output_str = &base_fixtures.str_out;

	return test_data;
}

bool call_test(string name, std::function<bool(test_data&)> fntest) {
	cout << endl << name << ":\n";
	test_data test_data = create_test_data();

	auto startTime = clock();
	bool res = fntest(test_data);
	auto endTime = clock();

	cout << "TIME: " << endTime - startTime << "ms" << endl;
	return res;
}

bool test_read_to_pool() {
	return call_test(__PRETTY_FUNCTION__, [](test_data& test_data) {
		ip_pool<Ip> ips;
		PoolCollection<Ip>::read_to_pool(test_data.in, ips);
		// Number of ip into ip_filter.tsv
		return ips.size() == fixtures_ip_count;
		});
}

void print_ip(const std::array<Byte, 4>& ip) {
	cout << endl << "{ ";
	for (int i = 0; i < 4; i++) {
		if (i) cout << ".";
		cout << (int)ip[i];
	}
	cout << " }" << endl;
}

//// Preparing vector<Ip>
void pool_fill(Ip* ips_source, ip_pool<Ip>& ip_pool, int max) {
	nested_loop(std::vector<int>{0, 0, 0, 0}, max, [&ip_pool, &ips_source, &max](const std::vector<int>& counters) {
		auto p1 = counters[0];
		auto p2 = counters[1];
		auto p3 = counters[2];
		auto p4 = counters[3];
		size_t idx =
			(p1 * max * max * max)
			+ (p1 * max * max)
			+ (p1 * max)
			+ p4;
		auto& v = ips_source[idx];
		v[0] = p1;
		v[1] = p2;
		v[2] = p3;
		v[3] = p4;
		ip_pool.push_back(v);
		return true;
		});
}

template<typename T>
std::vector<Ip> to_vector(T& list) {
	std::vector<Ip> vec;
	for (Ip ip : list)
		vec.push_back(ip);
	return vec;
}

void pool_shuffle(ip_pool<Ip>& pool) {
	std::random_device rd;
	std::mt19937 g(rd());
	if constexpr (std::is_same_v<std::vector<Ip>, decltype(pool)>) {
		shuffle(pool.begin(), pool.end(), g);
		return;
	}
	else if (std::is_same_v<std::list<Ip>, decltype(pool)>) {
		std::vector<Ip> vec = to_vector(pool);
		shuffle(vec.begin(), vec.end(), g);
		for (int i = 0; i < vec.size(); i++) {
			pool[i] = vec[i];
		}
		return;
	}

}
//// end Preparing vector<Ip>

bool check_sorted_pool(ip_pool<Ip>& ip_pool, int max) {
	return nested_loop(std::vector<int>{0, 0, 0, 0}, max, [&ip_pool, &max](const std::vector<int>& counters) {
		size_t idx = 0;
		for (int i = 0; i < 4; i++)
			idx += counters[i] * size_t(std::pow(max, (4 - 1) - i));
		auto& v = ip_pool[idx];

		for (int i = 0; i < 4; i++) {
			auto ip_part_wait = (max - 1) - counters[i];
			if (v[i] != ip_part_wait) {
				return false;
			}
		}
		return true;
		});
}

bool test_sort_of_pool() {
	const int factor = 20;
	const int ip_part_count = 256 / factor;
	const int count = ip_part_count * ip_part_count * ip_part_count * ip_part_count;
	std::array<Byte, 4> ips[count];
	ip_pool<std::array<Byte, 4>> fixture_pool;
	/*std::vector<std::array<Byte, 4>> fixture_pool;*/
	/*fixture_pool.reserve(count);*/
	pool_fill(ips, fixture_pool, ip_part_count);

	// Shuffle
	pool_shuffle(fixture_pool);

	cout << endl << __PRETTY_FUNCTION__ << ":\n";
	cout << "test params: factor = " << factor << ", count = " << count << endl;
	test_data test_data = create_test_data();

	// Sorting
	PoolCollection<Ip> poolCol;
	auto startTime = clock();
	poolCol.base_sort(&fixture_pool);
	auto endTime = clock();

	cout << "TIME: " << endTime - startTime << "ms" << endl;
	return check_sorted_pool(fixture_pool, ip_part_count);
}

bool test_of_true_filtering() {
	return call_test(__PRETTY_FUNCTION__, [](test_data& test_data) {
		std::ostringstream outres;
		run(test_data.in, outres);
		return outres.str() == *test_data.ptr_output_str;
		});
}

BOOST_AUTO_TEST_SUITE(ip_filter_test_suite)
INIT(init_base_fixtures)

BOOST_AUTO_TEST_CASE(test_of_test_system)
{
	BOOST_CHECK(test_read_to_pool());
	BOOST_CHECK(test_sort_of_pool());
	BOOST_CHECK(test_of_true_filtering());
}

BOOST_AUTO_TEST_SUITE_END()
