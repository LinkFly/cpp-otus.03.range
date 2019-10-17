#include <iostream>
#include "ip_filter_lib.h"

int main(int argc, char const *argv[])
{	
	/*std::cout << "--------- ============ -----------" << std::endl;
	exit(0);*/
	/*processInput();
	exit(0);*/

    try
    {
		PoolCollection<vecstr> ip_pools_col;

		read_lines(std::cin, [&ip_pool=ip_pools_col.base_pool](std::string line) {
			add_line_to_pool(ip_pool, line);
		});
		
        //  reverse lexicographically sort
		ip_pools_col.base_sort();

		// Prepare classified pools
		ip_pools_col.classify();

		//// Output
		/*std::ostringstream outres;*/
		auto ip_pools = ip_pools_col.get();
		output_pools<vecstr>(std::cout, ip_pools, [](vecstr ip_desc){
			return unpack_ip(ip_desc);
			});
		
		/*std::cout << outres.str() << std::endl;
		auto vec = read_file("C:\\Users\\Asus\\Dropbox\\courses\\cpp-otus\\03\\02.ip_filter\\ip_filter.tst");
		auto bigstr = lines_str(vec);
		std::cout << true << " " << (bigstr == outres.str());
		*/

       /* for(std::vector<std::vector<std::string> >::const_iterator ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
        {
            for(std::vector<std::string>::const_iterator ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
            {
                if (ip_part != ip->cbegin())
                {
                    std::cout << ".";

                }
                std::cout << *ip_part;
            }
            std::cout << std::endl;
        }*/

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        // ip = filter(46, 70)

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        // ip = filter_any(46)

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
