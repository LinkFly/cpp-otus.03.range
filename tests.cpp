#define BOOST_TEST_MODULE ipfilter_test_module

#include <boost/test/unit_test.hpp>
#include <fstream>
//#include <filesystem>
#include <boost/filesystem.hpp>

#include "ip_filter_lib.h"
#include "utils.h"

using std::cout;
using std::endl;

bool test() {return true;}

std::string lines_str(vecstr vec) {
	std::ostringstream sout;
	for (auto s : vec) {
		sout << s << "\n";
	}
	return sout.str();
}

vecstr read_file(std::string filename) {
	vecstr vec;
	std::ifstream fin(filename, std::ios::in);
	read_lines(fin, [&vec](std::string line) {
		vec.push_back(line);
		});
	return vec;
}

bool test_of_bin_search_functions() {
	vecint vec{ 44, 7, 6, 6, 5, 3, 2, 2, 1, 0};
	auto test_finded = [](const auto& vec, int val, int wait_pos) {
		auto identity = [](auto val) {return val; };
		auto itTo = lower_bound_with_convertor(vec.begin(), vec.end(), val, identity);
		auto pos = itTo - vec.begin();
		if (itTo == vec.end()) {
			pos = -1;
		}
		cout << "eq with end = " << (itTo == vec.end()) << endl;
		return std::tuple<int, int>{pos == wait_pos, pos};
	};
	auto out_vec = [](const auto& vec) {
		cout << "vector:\t[ ";
		for (auto val : vec) cout << val << " ";
		cout << "]\n";
	};
	auto test_finded_variants = [&test_finded, &out_vec](const auto& vec, const vec_vecint& fixtures) {
		for (auto val_waitpos : fixtures) {
			auto [res, pos] = test_finded(vec, val_waitpos[0], val_waitpos[1]);
			out_vec(vec);
			cout << "val: " << val_waitpos[0] << " pos: " << pos << " waitpos: " << val_waitpos[1] << " res: " << res << endl << endl;
			if (!res) return false;
		}
		return true;
	};
	cout << "----- Tests of lower_bound_with_convertor -----\n\n";
	vec_vecint fixtures{
		{2, 6},
		{6, 2},
		{44, 0},
		{1, 8},
	};
	auto res = test_finded_variants(vec, fixtures);
	if (!res) return false;
	res = test_finded_variants(vecint{ 2 }, vec_vecint{ {2, 0} });
	if (!res) return false;
	res = test_finded_variants(vecint{}, vec_vecint{ {2, -1} });
	if (!res) return false;
	res = test_finded_variants(vecint{4}, vec_vecint{ {2, -1} });
	if (!res) return false;
	cout << "-----------------------------------\n";
	return true;
}

bool test_of_true_filtering() {
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
  BOOST_CHECK(test_of_bin_search_functions());
}

BOOST_AUTO_TEST_SUITE_END()
