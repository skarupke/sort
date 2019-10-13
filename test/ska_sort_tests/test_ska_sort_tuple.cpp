#include <boost/sort/ska_sort/ska_sort_tuple.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

void test_tuple()
{
    std::vector<std::tuple<bool, int, bool>> data =
    {
        std::tuple<bool, int, bool>{ true, 5, true },
        std::tuple<bool, int, bool>{ true, 5, false },
        std::tuple<bool, int, bool>{ false, 6, false },
        std::tuple<bool, int, bool>{ true, 7, true },
        std::tuple<bool, int, bool>{ true, 4, false },
        std::tuple<bool, int, bool>{ false, 4, true },
        std::tuple<bool, int, bool>{ false, 5, false }
    };
    test_default_ska_sort_combinations(data);
}
void test_tuple_single()
{
    std::vector<std::tuple<int>> data =
    {
        std::tuple<int>{ 5 },
        std::tuple<int>{ -5 },
        std::tuple<int>{ 6 },
        std::tuple<int>{ 7 },
        std::tuple<int>{ 4 },
        std::tuple<int>{ 4 },
        std::tuple<int>{ 5 }
    };
    test_default_ska_sort_combinations(data);
}

void test_empty_typle()
{
    std::vector<std::tuple<>> data;
    data.emplace_back();
    data.emplace_back();
    test_default_ska_sort_combinations(data);
}

void test_tuple_reference()
{
    std::vector<std::tuple<bool, int, bool>> data = { std::tuple<bool, int, bool>{ true, 5, true }, std::tuple<bool, int, bool>{ true, 5, false }, std::tuple<bool, int, bool>{ false, 6, false }, std::tuple<bool, int, bool>{ true, 7, true }, std::tuple<bool, int, bool>{ true, 4, false }, std::tuple<bool, int, bool>{ false, 4, true }, std::tuple<bool, int, bool>{ false, 5, false } };
    test_default_ska_sort_combinations(data, [](auto & t) -> decltype(auto) { return t; });
}

void test_nested_tuple()
{
    std::vector<std::tuple<bool, std::pair<int, std::pair<int, int>>, std::tuple<bool, bool, bool>>> to_sort;
    to_sort.emplace_back(true, std::make_pair(5, std::make_pair(6, 7)), std::make_tuple(true, false, true));
    to_sort.emplace_back(false, std::make_pair(5, std::make_pair(6, 7)), std::make_tuple(true, false, true));
    to_sort.emplace_back(false, std::make_pair(5, std::make_pair(6, 8)), std::make_tuple(true, false, true));
    to_sort.emplace_back(false, std::make_pair(5, std::make_pair(6, 6)), std::make_tuple(true, false, true));
    to_sort.emplace_back(false, std::make_pair(5, std::make_pair(7, 6)), std::make_tuple(true, false, true));
    to_sort.emplace_back(false, std::make_pair(5, std::make_pair(7, 6)), std::make_tuple(true, true, true));
    to_sort.emplace_back(false, std::make_pair(5, std::make_pair(7, 6)), std::make_tuple(true, true, false));
    to_sort.emplace_back(false, std::make_pair(5, std::make_pair(7, 6)), std::make_tuple(false, true, false));
    to_sort.emplace_back(false, std::make_pair(4, std::make_pair(7, 6)), std::make_tuple(false, true, false));
    test_default_ska_sort_combinations(to_sort);
}

void test_vector_tuple_string()
{
    std::vector<std::tuple<std::string, std::string>> to_sort;
    to_sort.emplace_back("hi", "there");
    to_sort.emplace_back("you", "are");
    to_sort.emplace_back("probably", "not");
    to_sort.emplace_back("going", "to");
    to_sort.emplace_back("pass", "");
    to_sort.emplace_back("", "");
    to_sort.emplace_back("", "this");
    to_sort.emplace_back("test", "the");
    to_sort.emplace_back("first", "time");
    to_sort.emplace_back("oh it did pass", "n");
    to_sort.emplace_back("e", "a");
    to_sort.emplace_back("t!", "");
    to_sort.emplace_back("hi", "there");
    to_sort.emplace_back("I", "added");
    to_sort.emplace_back("more", "tests");
    to_sort.emplace_back("hi", "there");
    to_sort.emplace_back("needed", "the");
    to_sort.emplace_back("same", "prefix");
    test_default_ska_sort_combinations(to_sort);
}
void test_vector_vector_tuple_string()
{
    std::vector<std::vector<std::tuple<std::string, std::string>>> to_sort;
    to_sort.emplace_back(std::vector<std::tuple<std::string, std::string>>
    {
        {"hi", "there"},
        {"you", "are"},
        {"probably", "not"}
    });
    to_sort.emplace_back(std::vector<std::tuple<std::string, std::string>>
    {
        {"going", "to"},
        {"pass", ""},
        {"", ""}
    });
    to_sort.emplace_back(std::vector<std::tuple<std::string, std::string>>
    {
        {"", "this"},
        {"test", "the"},
        {"first", "time"},
    });
    to_sort.emplace_back(std::vector<std::tuple<std::string, std::string>>
    {
        {"oh it did pass", "n"},
        {"e", "a"},
        {"t!", ""},
    });
    to_sort.emplace_back(std::vector<std::tuple<std::string, std::string>>
    {
        {"hi", "there"},
        {"I", "added"},
        {"more", "tests"},
    });
    to_sort.emplace_back(std::vector<std::tuple<std::string, std::string>>
    {
        {"hi", "there"},
        {"needed", "the"},
        {"same", "prefix"},
    });
    test_default_ska_sort_combinations(to_sort);
}

struct MovableString : private std::string
{
    using std::string::string;
    MovableString() = default;
    MovableString(const MovableString &) = delete;
    MovableString & operator=(const MovableString &) = delete;
    MovableString(MovableString &&) = default;
    MovableString & operator=(MovableString &&) = default;

    friend bool operator<(const MovableString & lhs, const MovableString & rhs)
    {
        return static_cast<const std::string &>(lhs) < static_cast<const std::string &>(rhs);
    }

    friend const std::string & to_radix_sort_key(const MovableString & str)
    {
        return static_cast<const std::string &>(str);
    }

    char first_char() const
    {
        return (*this)[0];
    }
};

struct Customer
{
    Customer() = default;
    Customer(MovableString && first, MovableString && second)
        : first_name(std::move(first)), last_name(std::move(second))
    {
    }

    MovableString first_name;
    MovableString last_name;
};

void test_no_copy()
{
    // sorting customers by last name then first name
    // I want to return references. I use a MovableString
    // to make it easier to get an error message when copying
    // happens
    std::vector<Customer> to_sort;
    to_sort.emplace_back("a", "b");
    to_sort.emplace_back("foo", "bar");
    to_sort.emplace_back("g", "a");
    to_sort.emplace_back("w", "d");
    to_sort.emplace_back("b", "c");
    test_default_ska_sort_combinations(to_sort, [](const Customer & customer)
    {
        return std::tie(customer.last_name, customer.first_name);
    });
    test_small_key_ska_sort_combinations(to_sort, [](const Customer & customer)
    {
        return customer.first_name.first_char();
    });
}

int test_main(int, char **)
{
    test_tuple();
    test_tuple_single();
    test_empty_typle();
    test_tuple_reference();
    test_nested_tuple();
    test_vector_tuple_string();
    test_vector_vector_tuple_string();
    test_no_copy();

    return 0;
}


