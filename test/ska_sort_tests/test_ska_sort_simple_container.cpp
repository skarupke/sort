#include <boost/sort/ska_sort/ska_sort_base.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

#include <deque>
#include <vector>
#include <list>

void test_deque()
{
    std::deque<std::deque<int>> to_sort =
    {
        { 1, 2, 3 },
        { 1, 2, 4 },
        { 1, 2, 2 },
        { 1, 2 },
        { 2, 3, 4 },
        { 0, -1, 2 },
        { 5, 4, 3 },
        { -1, -2, 5 },
        { 5, 3, 4 },
        { -1, -3, 5, 6 },
    };
    test_default_ska_sort_combinations(to_sort);
}

void test_vector()
{
    std::vector<std::vector<int>> to_sort =
    {
        { 1, 2, 3 },
        { 1, 2, 2 },
        { 1, 3, 2 },
        { 2, 3, 2 },
        { 2, 3, 2, 4 },
        { 2, 3, 2, 4, 5 },
        { 10002, 3, 2, 4, 5 },
        { 10001, 3, 2, 4, 5 },
        { 3, 2, 4, 5 },
        { 1 },
        {},
    };
    test_default_ska_sort_combinations(to_sort);
}

void test_vector_faster_compare()
{
    std::vector<std::vector<int>> to_sort =
    {
        { 115, -61, -87 },
        { 115, 108, 97 },
        { 115, 116, 101 },
        { 115, 110, 101 },
        { 115, 119, 101 },
        { 115, 116, 101 },
        { 115, 97, 103 },
        { 115, 105, 103 },
        { 115, 99, 104 },
        { 115, 104, 105 },
        { 115, 97, 105 },
        { 115, 111, 109 },
        { 115, 105, 110 },
        { 115, 108, 111 },
        { 115, 111, 117 },
        { 115, 97, 118 },
    };
    test_default_ska_sort_combinations(to_sort);
}

void test_string_in_vector()
{
    std::vector<std::vector<std::string>> to_sort =
    {
        { "hi", "there", "you" },
        { "are", "probably", "not", "going" },
        { "to", "pass" },
        { "" },
        { },
        { "this", "test", "the", "first" },
        { "time" },
        { "oh it did pass", "n", "e", "a", "t!" },
        { "hi", "there", "I", "added", "more", "tests" },
        { "hi", "there", "needed", "the", "same", "prefix" },
    };
    test_default_ska_sort_combinations(to_sort);
}

void test_std_array()
{
    std::vector<std::array<float, 4>> to_sort =
    {
        {{ 1.0f, 2.0f, 3.0f, 4.0f }},
        {{ 0.0f, 3.0f, 4.0f, 5.0f }},
        {{ 1.0f, 1.5f, 2.0f, 2.5f }},
        {{ 1.0f, 2.0f, 2.5f, 4.0f }},
        {{ 1.0f, 2.0f, 2.5f, 3.5f }},
        {{ 0.0f, 3.0f, 4.5f, 4.5f }}
    };
    test_default_ska_sort_combinations(to_sort);
}


void test_vector_of_list()
{
    std::vector<std::list<int>> to_sort =
    {
        { 1, 2, 3 },
        { 1, 2, 2 },
        { 0, 1, 2 }
    };
    test_default_ska_sort_combinations(to_sort);
}

int test_main(int, char **)
{
    test_deque();
    test_vector();
    test_vector_faster_compare();
    test_string_in_vector();
    test_std_array();
    test_vector_of_list();

    return 0;
}


