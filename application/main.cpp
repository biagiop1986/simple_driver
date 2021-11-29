#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

#include "user_driver.h"

using namespace user_driver;

int main(const int argc, const char* argv[])
{
	accid_t acc_id = 0;

    const int num_iters = 100000;
    std::vector<unsigned long long> elaps_res;
    std::vector<unsigned long long> elaps_rel;

	elaps_res.reserve(num_iters);
	elaps_rel.reserve(num_iters);

    for(int i=0; i<num_iters; ++i)
    {
        auto start = std::chrono::steady_clock::now();
		reserve(acc_id);
        auto stop = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        elaps_res.push_back(elapsed);

        start = std::chrono::steady_clock::now();
		release(acc_id);
        stop = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        elaps_rel.push_back(elapsed);
    }
    auto avg_reserve = std::accumulate(elaps_res.begin(), elaps_res.end(), 0ull) / elaps_res.size();
    auto avg_release = std::accumulate(elaps_rel.begin(), elaps_rel.end(), 0ull) / elaps_rel.size();

    std::cout << "Elapsed time reserve: " << avg_reserve << " ns" << std::endl;
    std::cout << "Elapsed time release: " << avg_release << " ns" << std::endl;

	return 0;
}
