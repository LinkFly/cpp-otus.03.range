#define BOOST_TEST_MODULE ipfilter_test_module

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <filesystem>

#include "ip_filter_lib.h"


bool test() {return true;}

bool test_of_true_filtering() {
	std::filesystem::path cwd = std::filesystem::current_path();
	auto test_files_dir = cwd / ".." / "..";

	auto file_in = (test_files_dir / "ip_filter.tsv").string();
	auto file_test = (test_files_dir / "ip_filter.tst").string();

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
