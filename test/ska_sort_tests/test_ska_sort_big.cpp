#include <boost/sort/ska_sort/ska_sort_base.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

#include <vector>
#include <random>

template<typename T, typename RandomnessLimits = T, typename Randomness>
void test_uniform_distribution(Randomness & randomness)
{
    std::vector<T> to_sort;
    to_sort.reserve(1'000'000);
    std::uniform_int_distribution<T> distribution(std::numeric_limits<RandomnessLimits>::lowest(), (std::numeric_limits<RandomnessLimits>::max)());
    while (to_sort.size() != to_sort.capacity())
    {
        to_sort.push_back(distribution(randomness));
    }
    test_default_ska_sort_combinations(to_sort);
}

template<typename T, typename Randomness>
void test_exponential_distribution(Randomness & randomness)
{
    std::vector<T> to_sort;
    to_sort.reserve(1'000'000);
    std::exponential_distribution<T> distribution(1 / 1024.0f);
    std::uniform_int_distribution<int> random_sign(0, 1);
    while (to_sort.size() != to_sort.capacity())
    {
        to_sort.push_back(distribution(randomness));
        if (random_sign(randomness))
            to_sort.back() = -to_sort.back();
    }
    test_default_ska_sort_combinations(to_sort);
}


int test_main(int, char **)
{
    std::mt19937_64 randomness(5);
    test_uniform_distribution<int, uint8_t>(randomness);
    test_uniform_distribution<uint16_t>(randomness);
    test_uniform_distribution<int>(randomness);
    test_uniform_distribution<int64_t>(randomness);
    test_uniform_distribution<uint64_t>(randomness);

    test_exponential_distribution<float>(randomness);
    test_exponential_distribution<double>(randomness);
    test_exponential_distribution<long double>(randomness);

    return 0;
}


