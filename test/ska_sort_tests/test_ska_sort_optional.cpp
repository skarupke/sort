#include <boost/sort/ska_sort/ska_sort_optional.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

void test_optional()
{
    std::vector<std::optional<int>> to_sort;
    to_sort.emplace_back(5);
    to_sort.emplace_back(std::nullopt);
    to_sort.emplace_back(6);
    to_sort.emplace_back(-3);
    to_sort.emplace_back(2);
    to_sort.emplace_back(7);
    to_sort.emplace_back(std::nullopt);
    test_default_ska_sort_combinations(to_sort);
}

void test_optional_in_pair()
{
    std::vector<std::pair<std::optional<int>, int>> to_sort;
    to_sort.emplace_back(5, 5);
    to_sort.emplace_back(std::nullopt, 4);
    to_sort.emplace_back(std::nullopt, 3);
    to_sort.emplace_back(std::nullopt, 8);
    to_sort.emplace_back(5, 2);
    to_sort.emplace_back(5, 100);
    to_sort.emplace_back(-5, 100);
    to_sort.emplace_back(-5, 2);
    to_sort.emplace_back(-5, 3);
    test_default_ska_sort_combinations(to_sort);
}

void test_optional_in_vector()
{
    std::vector<std::vector<std::optional<int>>> to_sort =
    {
        { 5, 6, 2, std::nullopt },
        { 5, 3, 2, std::nullopt },
        { 5, std::nullopt, 2, std::nullopt },
        { 5, std::nullopt, 4, std::nullopt },
        { 5, std::nullopt, 1, std::nullopt },
        { std::nullopt, std::nullopt, 1, std::nullopt },
        { std::nullopt, std::nullopt, 0, std::nullopt },
    };
    test_default_ska_sort_combinations(to_sort);
}
#include <boost/optional.hpp>

void test_boost_optional()
{
    std::vector<boost::optional<int>> to_sort;
    to_sort.emplace_back(5);
    to_sort.emplace_back(boost::none);
    to_sort.emplace_back(6);
    to_sort.emplace_back(-3);
    to_sort.emplace_back(2);
    to_sort.emplace_back(7);
    to_sort.emplace_back(boost::none);
    test_default_ska_sort_combinations(to_sort);
}

int test_main(int, char **)
{
    test_optional();
    test_optional_in_pair();
    test_optional_in_vector();
    test_boost_optional();

    return 0;
}


