#define BOOST_TEST_MODULE ipfilter_test_module

#include <boost/test/unit_test.hpp>
#include <fstream>
//#include <filesystem>

#include "ip_filter_lib.h"


bool test() {return true;}

bool test_of_true_filtering() {
	/*std::filesystem::path cwd = std::filesystem::current_path();*/
	//auto data_file = cwd / ".." / ".." 

	// TODO!!! Избавиться от абсолютного пути
	auto file_in = "C:\\Users\\Asus\\Dropbox\\courses\\cpp-otus\\03\\02.ip_filter\\ip_filter.tsv";
	auto file_test = "C:\\Users\\Asus\\Dropbox\\courses\\cpp-otus\\03\\02.ip_filter\\ip_filter.tst";

	std::ifstream fin(file_in, std::ios::in);
	std::ostringstream outres;
	run(fin, outres);
	
	auto vec = read_file(file_test);
	auto bigstr = lines_str(vec);
	return bigstr == outres.str();
}

BOOST_AUTO_TEST_SUITE(ip_filter_test_suite)

BOOST_AUTO_TEST_CASE(test_of_test_system)
{
  BOOST_CHECK(test());
  BOOST_CHECK(test_of_true_filtering());
}

BOOST_AUTO_TEST_SUITE_END()
