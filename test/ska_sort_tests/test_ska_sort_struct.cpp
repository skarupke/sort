#include <boost/sort/ska_sort/ska_sort_base.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

#include <vector>
#include <array>

void test_pair()
{
    std::vector<std::pair<short, int>> data =
    {
        { 1, 0 },
        { 1, 2 },
        { 1, -3 },
        { 10, 2 },
        { 1, -4 },
        { 10, 0 },
        { -10, 7 },
        { -10, -10 },
        { 10, 10 }
    };
    test_default_ska_sort_combinations(data);
}

void test_pair_bool()
{
    std::vector<std::pair<int, bool>> to_sort = { { 5, true }, { 5, false }, { 6, false }, { 7, true }, { 4, false }, { 4, true } };
    test_default_ska_sort_combinations(to_sort);
}
void test_pair_other_direction()
{
    std::vector<std::pair<bool, int>> to_sort = { { true, 5 }, { false, 5 }, { false, 6 }, { true, 7 }, { false, 4 }, { true, 4 } };
    test_default_ska_sort_combinations(to_sort);
}
void test_pair_reference()
{
    std::vector<std::pair<int, bool>> to_sort = { { 5, true }, { 5, false }, { 6, false }, { 7, true }, { 4, false }, { 4, true } };
    test_default_ska_sort_combinations(to_sort, [](auto & i) -> decltype(auto) { return i; });
}

void test_struct()
{
    struct ToSort
    {
        int a = 0;
    };
    std::vector<ToSort> to_sort =
    {
        { 1 },
        { -1 },
        { 5 },
        { 500 },
        { -500 },
        { -5000 },
        { 5000 },
        { 50000 },
        { -50000 },
        { 7 },
    };
    test_default_ska_sort_combinations(to_sort, [](const ToSort & to_sort)
    {
        return to_sort.a;
    });
    test_small_key_ska_sort_combinations(to_sort, [](const ToSort & to_sort)
    {
        return to_sort.a;
    });
}

void test_struct_mem_ptr()
{
    struct ToSort
    {
        int a = 0;
    };
    std::vector<ToSort> to_sort =
    {
        { 1 },
        { -1 },
        { 5 },
        { 500 },
        { -500 },
        { -5000 },
        { 5000 },
        { 50000 },
        { -50000 },
        { 7 },
    };
    test_default_ska_sort_combinations(to_sort, &ToSort::a);
    test_small_key_ska_sort_combinations(to_sort, &ToSort::a);
}


void test_small_key()
{
    struct ToSort
    {
        int i = 0;
        std::array<int, 100> more;
        ToSort(int to_fill)
            : i(to_fill)
        {
            more.fill(to_fill);
        }
    };

    std::vector<ToSort> to_sort =
    {
        5, 6, 700, 1000, 10000,
        -5, 0, -100, 70, -100000,
        1001, 1002, 1005, 1003, 999,
        12, 24
    };
    test_small_key_ska_sort_combinations(to_sort, [](const ToSort & to_sort){ return to_sort.i; });
}


int test_main(int, char **)
{
    test_pair();
    test_pair_bool();
    test_pair_other_direction();
    test_pair_reference();
    test_struct();
    test_struct_mem_ptr();
    test_small_key();

    return 0;
}


