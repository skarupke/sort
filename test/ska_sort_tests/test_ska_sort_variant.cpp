#include <boost/sort/ska_sort/ska_sort_variant.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

struct ThrowingType
{
    operator int() const
    {
        throw 5;
    }
};

void test_variant()
{
    std::vector<std::variant<int, std::string, float>> to_sort;
    to_sort.emplace_back(5);
    to_sort.emplace_back(0);
    to_sort.emplace_back("foo");
    to_sort.emplace_back("foo");
    to_sort.emplace_back("foo");
    to_sort.emplace_back("bar");
    to_sort.emplace_back(1.0f);
    to_sort.emplace_back(-2.0f);
    to_sort.emplace_back(-1);
    to_sort.emplace_back("baz3");
    to_sort.emplace_back("baz");
    to_sort.emplace_back("baz2");
    to_sort.emplace_back(7.0f);
    BOOST_CHECK_THROW(to_sort[2].emplace<0>(ThrowingType()), int);
    BOOST_CHECK(to_sort[2].valueless_by_exception());
    BOOST_CHECK_THROW(to_sort[3].emplace<0>(ThrowingType()), int);
    BOOST_CHECK(to_sort[3].valueless_by_exception());
    test_default_ska_sort_combinations(to_sort);
}

void test_variant_single()
{
    std::vector<std::variant<int>> to_sort;
    to_sort.emplace_back(5);
    to_sort.emplace_back(0);
    to_sort.emplace_back(4);
    to_sort.emplace_back(4);
    to_sort.emplace_back(4);
    to_sort.emplace_back(3);
    to_sort.emplace_back(1);
    to_sort.emplace_back(-2);
    to_sort.emplace_back(-1);
    to_sort.emplace_back(3);
    to_sort.emplace_back(1);
    to_sort.emplace_back(2);
    to_sort.emplace_back(7);
    BOOST_CHECK_THROW(to_sort[2].emplace<0>(ThrowingType()), int);
    BOOST_CHECK(to_sort[2].valueless_by_exception());
    BOOST_CHECK_THROW(to_sort[3].emplace<0>(ThrowingType()), int);
    BOOST_CHECK(to_sort[3].valueless_by_exception());
    test_default_ska_sort_combinations(to_sort);
}

void test_variant_in_pair()
{
    std::vector<std::pair<std::variant<int, float, std::string>, int>> to_sort;
    to_sort.emplace_back(5, 5);
    to_sort.emplace_back(0, 4);
    to_sort.emplace_back("foo", 2);
    to_sort.emplace_back("foo", 1);
    to_sort.emplace_back("foo", 3);
    to_sort.emplace_back(5, 3);
    to_sort.emplace_back(5, 1);
    to_sort.emplace_back(0, 8);
    to_sort.emplace_back(0, 2);
    to_sort.emplace_back(1.0f, 2);
    to_sort.emplace_back(1.0f, 21);
    to_sort.emplace_back(-2.0f, 3);
    to_sort.emplace_back(-2.0f, 4);
    to_sort.emplace_back(1.0f, -3);
    to_sort.emplace_back(1.0f, 3);
    to_sort.emplace_back(-2.0f, 2);
    BOOST_CHECK_THROW(to_sort[2].first.emplace<0>(ThrowingType()), int);
    BOOST_CHECK(to_sort[2].first.valueless_by_exception());
    BOOST_CHECK_THROW(to_sort[3].first.emplace<0>(ThrowingType()), int);
    BOOST_CHECK(to_sort[3].first.valueless_by_exception());
    BOOST_CHECK_THROW(to_sort[4].first.emplace<0>(ThrowingType()), int);
    BOOST_CHECK(to_sort[4].first.valueless_by_exception());
    test_default_ska_sort_combinations(to_sort);
}

void test_variant_in_vector()
{
    std::vector<std::vector<std::variant<int, float, std::string>>> to_sort =
    {
        { 5, 6, 2, 1.0f, 3.1f },
        { 5.0f, 6.0f, 2, 1, 3.1f },
        { 5.0f, 6, 2.0f, 1, 3.1f },
        { 5, 6.0f, 2, 1, 3 },
        { 5, 6.0f, 2, 1, 3.1f },
        { 5.0f, 6, 2.1f, 1, 3 },
        { 5.0f, 6, 1.9f, 1, 3.1f },
        { 5.0f, 6, 1.9f, 1, 3.1f, "foo2" },
        { 5.0f, 6, 1.9f, 1, 3.1f, "foo3" },
        { 5.0f, 6, 1.9f, 1, 3.1f, "foo4" },
        { 5.0f, 6, 1.9f, 1, 3.1f, "foo1" },
    };
    test_default_ska_sort_combinations(to_sort);
}

int test_main(int, char **)
{
    test_variant();
    test_variant_single();
    test_variant_in_pair();
    test_variant_in_vector();

    return 0;
}


