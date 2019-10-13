#include <boost/sort/ska_sort/ska_sort_memory.hpp>

#include <numeric>
#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

void test_unique_ptr()
{
    std::vector<std::unique_ptr<int>> to_sort;
    to_sort.push_back(std::make_unique<int>(4));
    to_sort.push_back(std::make_unique<int>(2));
    to_sort.push_back(std::make_unique<int>(1));
    to_sort.push_back(std::make_unique<int>(6));
    to_sort.push_back(std::make_unique<int>(0));
    to_sort.push_back(std::make_unique<int>(-1));
    to_sort.push_back(std::make_unique<int>(-100));
    to_sort.push_back(nullptr);
    test_default_ska_sort_combinations(to_sort);
}

void test_shared_ptr()
{
    std::vector<std::shared_ptr<int>> to_sort =
    {
        std::make_shared<int>(4),
        std::make_shared<int>(2),
        std::make_shared<int>(1),
        std::make_shared<int>(6),
        std::make_shared<int>(0),
        std::make_shared<int>(-1),
        std::make_shared<int>(-100),
        nullptr,
    };
    test_default_ska_sort_combinations(to_sort);
}

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

void test_boost_shared_ptr()
{
    std::vector<boost::shared_ptr<int>> to_sort =
    {
        boost::make_shared<int>(4),
        boost::make_shared<int>(2),
        boost::make_shared<int>(1),
        boost::make_shared<int>(6),
        boost::make_shared<int>(0),
        boost::make_shared<int>(-1),
        boost::make_shared<int>(-100),
        nullptr,
    };
    test_default_ska_sort_combinations(to_sort);
}

int test_main(int, char **)
{
    test_unique_ptr();
    test_shared_ptr();
    test_boost_shared_ptr();

    return 0;
}


