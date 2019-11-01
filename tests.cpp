#define BOOST_TEST_MODULE ipfilter_test_module

#include <boost/test/unit_test.hpp>
#include <fstream>
//#include <filesystem>
#include <boost/filesystem.hpp>
#include <ctime>
//#include <dos.h>
#include <chrono>
#include <thread>

#include "ip_filter_lib.h"
#include "utils.h"

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

string read_file(std::string filename) {
	std::ifstream fin(filename, std::ios::in);
	string res;
	for (std::string line; std::getline(fin, line);)
		res += (line + "\n");
	return res;
}

std::tuple<std::string, std::string> get_fixtures_files() {
	/*std::filesystem::path cwd = std::filesystem::current_path();*/
	boost::filesystem::path cwd = boost::filesystem::current_path();
	auto test_files_dir = cwd / "..";

	auto file_in = (test_files_dir / "ip_filter.tsv").string();
	if (!boost::filesystem::exists(file_in)) {
		std::cerr << "not found file_in: " << file_in << std::endl;
		exit(2);
	}
	auto file_test = (test_files_dir / "ip_filter.tst").string();
	if (!boost::filesystem::exists(file_test)) {
		std::cerr << "not found file_test: " << file_test << std::endl;
		exit(2);
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

//bool test_read_ips() {
//	
//	PoolCollection<Ip> poolCol;
//	return false;
//}
bool test_of_true_filtering() {
	test_data test_data = create_test_data();

	std::ostringstream outres;
	auto startTime = clock();
	run(test_data.in, outres);
	auto endTime = clock();
	auto res = outres.str() == *test_data.ptr_output_str;
	
	cout << "TIME: " << endTime - startTime << "ms" << endl;
	
	return res;
}

BOOST_AUTO_TEST_SUITE(ip_filter_test_suite)


BOOST_AUTO_TEST_CASE(test_of_test_system)
{
	init_base_fixtures();
	BOOST_CHECK(test_of_true_filtering());
}

BOOST_AUTO_TEST_SUITE_END()
