#include <boost/sort/ska_sort/ska_sort_base.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

#include <deque>
#include <vector>
#include <list>

struct MovableInt
{
    MovableInt()
        : i()
    {
    }

    MovableInt(int i)
        : i(i)
    {
    }
    MovableInt(MovableInt &&) = default;
    MovableInt & operator=(MovableInt &&) = default;
    MovableInt(const MovableInt &) = delete;
    MovableInt & operator=(const MovableInt &) = delete;

    friend bool operator==(const MovableInt & l, const MovableInt & r)
    {
        return l.i == r.i;
    }
    friend bool operator<(const MovableInt & l, const MovableInt & r)
    {
        return l.i < r.i;
    }

    int i;
};
int to_radix_sort_key(const MovableInt & i)
{
    return i.i;
}

void test_movables()
{
    std::vector<MovableInt> to_sort;
    to_sort.emplace_back(1);
    to_sort.emplace_back(2);
    to_sort.emplace_back(0);
    to_sort.emplace_back(-1);
    to_sort.emplace_back(20);
    to_sort.emplace_back(-5);
    test_default_ska_sort_combinations(to_sort);
}

void test_vector_of_movables()
{
    std::vector<std::vector<MovableInt>> to_sort;
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(3);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(1); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(0); to_sort.back().emplace_back(1); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(3); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(2); to_sort.back().emplace_back(3); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(-2); to_sort.back().emplace_back(-3); to_sort.back().emplace_back(-4);
    test_default_ska_sort_combinations(to_sort);
}

void test_vector_of_vector_of_movables()
{
    std::vector<std::vector<std::vector<MovableInt>>> to_sort;
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(2); to_sort.back().back().emplace_back(3);
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    test_default_ska_sort_combinations(to_sort);
}

struct NestedMovable
{
    MovableInt movable;
    NestedMovable() = default;
    NestedMovable(int i)
        : movable(i)
    {
    }
    NestedMovable(NestedMovable &&) = default;
    NestedMovable & operator=(NestedMovable &&) = default;

    friend bool operator==(const NestedMovable & l, const NestedMovable & r)
    {
        return l.movable == r.movable;
    }
    friend bool operator<(const NestedMovable & l, const NestedMovable & r)
    {
        return l.movable < r.movable;
    }
};
const MovableInt & to_radix_sort_key(const NestedMovable & m)
{
    return m.movable;
}

void test_nested_movables()
{
    std::vector<NestedMovable> to_sort;
    to_sort.emplace_back(1);
    to_sort.emplace_back(2);
    to_sort.emplace_back(0);
    to_sort.emplace_back(-1);
    to_sort.emplace_back(20);
    to_sort.emplace_back(-5);
    test_default_ska_sort_combinations(to_sort);
}

void test_vector_of_nested_movables()
{
    std::vector<std::vector<NestedMovable>> to_sort;
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(3);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(1); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(0); to_sort.back().emplace_back(1); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(3); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(2); to_sort.back().emplace_back(3); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(-2); to_sort.back().emplace_back(-3); to_sort.back().emplace_back(-4);
    test_default_ska_sort_combinations(to_sort);
}

void test_list_of_nested_movables()
{
    std::vector<std::list<NestedMovable>> to_sort;
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(3);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(1); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(0); to_sort.back().emplace_back(1); to_sort.back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(2); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(1); to_sort.back().emplace_back(3); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(2); to_sort.back().emplace_back(3); to_sort.back().emplace_back(4);
    to_sort.emplace_back();
    to_sort.back().emplace_back(-2); to_sort.back().emplace_back(-3); to_sort.back().emplace_back(-4);
    test_default_ska_sort_combinations(to_sort);
}

void test_list_of_list_of_nested_movables()
{
    std::vector<std::list<std::list<NestedMovable>>> to_sort;
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(2); to_sort.back().back().emplace_back(3);
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    to_sort.back().back().emplace_back(1); to_sort.back().back().emplace_back(2);
    test_default_ska_sort_combinations(to_sort);
}


template<typename T, typename A = std::allocator<T>>
struct list_wrapper : std::list<T, A>
{
    using std::list<T, A>::list;
};
template<typename T, typename A = std::allocator<T>>
struct deque_wrapper : std::deque<T, A>
{
    using std::deque<T, A>::deque;
};

template<typename T, typename A>
struct boost::sort::ska_sort_container_access<list_wrapper<T, A>>
{
    static auto begin(const list_wrapper<T, A> & container)
    {
        return container.begin();
    }
    static auto end(const list_wrapper<T, A> & container)
    {
        return container.end();
    }
};
template<typename T, typename A>
struct boost::sort::ska_sort_container_access<deque_wrapper<T, A>>
{
    static bool used_container_access;
    static auto begin(const deque_wrapper<T, A> & container)
    {
        used_container_access = true;
        return container.begin();
    }
    static auto end(const deque_wrapper<T, A> & container)
    {
        used_container_access = true;
        return container.end();
    }
};
template<typename T, typename A>
bool boost::sort::ska_sort_container_access<deque_wrapper<T, A>>::used_container_access = false;

static const std::vector<std::vector<int>> container_sort_test_data =
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
    { 1, 2, 3 },
    { 1, 2, 3 },
    { 1, 2, 3 },
    { 1, 2, 3 },
    { 1, 2, 3 },
    { 1, 2, 3 },
};

void test_custom_container_list()
{
    std::vector<list_wrapper<int>> to_sort;
    for (const std::vector<int> & list : container_sort_test_data)
    {
        to_sort.emplace_back(list.begin(), list.end());
    }
    test_default_ska_sort_combinations(to_sort);
}
void test_custom_container_list_in_pair_first()
{
    std::vector<std::pair<list_wrapper<int>, bool>> to_sort;
    for (const std::vector<int> & list : container_sort_test_data)
    {
        to_sort.emplace_back(list_wrapper<int>(list.begin(), list.end()), to_sort.size() % 2 != 0);
    }
    test_default_ska_sort_combinations(to_sort);
}
void test_custom_container_list_in_pair_second()
{
    std::vector<std::pair<bool, list_wrapper<int>>> to_sort;
    for (const std::vector<int> & list : container_sort_test_data)
    {
        to_sort.emplace_back(to_sort.size() % 2 != 0, list_wrapper<int>(list.begin(), list.end()));
    }
    test_default_ska_sort_combinations(to_sort);
}

void test_vector_vector_vector()
{
    std::vector<std::vector<std::vector<std::vector<int>>>> to_sort;
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back();
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(2);
    to_sort.back().back().emplace_back();
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(3);
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back();
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(3);
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back();
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back();
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(3);
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(3);
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back();
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(2);
    to_sort.back().back().back().emplace_back(2); to_sort.back().back().back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back();
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(2);
    to_sort.emplace_back();
    to_sort.back().emplace_back();
    to_sort.back().back().emplace_back();
    to_sort.back().back().back().emplace_back(1); to_sort.back().back().back().emplace_back(2);
    test_default_ska_sort_combinations(to_sort);
}

void test_custom_container_deque()
{
    std::vector<deque_wrapper<int>> to_sort;
    for (const std::vector<int> & list : container_sort_test_data)
    {
        to_sort.emplace_back(list.begin(), list.end());
    }
    boost::sort::ska_sort_container_access<deque_wrapper<int>>::used_container_access = false;
    test_default_ska_sort_combinations(to_sort);
    BOOST_CHECK(boost::sort::ska_sort_container_access<deque_wrapper<int>>::used_container_access);
}

int test_main(int, char **)
{
    test_movables();
    test_vector_of_movables();
    test_vector_of_vector_of_movables();
    test_nested_movables();
    test_vector_of_nested_movables();
    test_list_of_nested_movables();
    test_list_of_list_of_nested_movables();
    test_custom_container_list();
    test_custom_container_list_in_pair_first();
    test_custom_container_list_in_pair_second();
    test_vector_vector_vector();
    test_custom_container_deque();

    return 0;
}


