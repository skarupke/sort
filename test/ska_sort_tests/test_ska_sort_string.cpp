#include <boost/sort/ska_sort/ska_sort_base.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

void test_string()
{
    std::vector<std::string> to_sort =
    {
        "Hi",
        "There",
        "Hello",
        "World!",
        "Foo",
        "Bar",
        "Baz",
        "",
    };
    test_default_ska_sort_combinations(to_sort);
}
void test_u16string()
{
    std::u16string to_sort = u"Hello, World!";
    test_default_ska_sort_combinations(to_sort);
}
void test_u32string()
{
    std::u32string to_sort = U"Hello, World!";
    test_default_ska_sort_combinations(to_sort);
}
void test_wstring()
{
    std::wstring to_sort = L"Hello, World!";
    test_default_ska_sort_combinations(to_sort);
}
void test_string_unicode()
{
    std::vector<std::string> to_sort =
    {
        "Hi",
        "There",
        "précis"
        "Hello",
        "prepayment",
        "World!",
        "scans",
        "séances",
        "Foo",
        "Bar",
        "née"
        "Baz",
        "naive"
        "",
        "some",
        "more",
        "strings",
        "to",
        "get",
        "over",
        ",",
        "16",
        "or",
        "32",
        "or some",
        "limit",
        "like",
        "that",
        "10000",
        "20000",
        "foo",
        "pirate",
        "rhino",
        "city",
        "blanket",
        "balloon",
    };
    test_default_ska_sort_combinations(to_sort);
    std::vector<std::string_view> as_views;
    as_views.reserve(to_sort.size());
    for (const std::string & str : to_sort)
    {
        as_views.emplace_back(str);
    }
    test_default_ska_sort_combinations(as_views);
}

void test_string_unicode_direct_compare()
{
    auto comparer = boost::sort::ska_sorter<std::string>::sort_at_index{1};
    BOOST_CHECK(comparer("née", "naive") == std::less<std::string>{}("née", "naive"));
    BOOST_CHECK(comparer("aaa", "aaaaa"));
    auto at_17 = boost::sort::ska_sorter<std::string>::sort_at_index{17};
    BOOST_CHECK(at_17("aaaaaaaaaaaaaaaaaaaab", "aaaaaaaaaaaaaaaaaaaabb") == std::less<std::string>{}("aaaaaaaaaaaaaaaaaaaab", "aaaaaaaaaaaaaaaaaaaabb"));
}

void test_string_bad_case()
{
    int limit = 500;
    std::vector<std::string> to_sort;
    for (int i = 0; i < limit; ++i)
    {
        to_sort.push_back("a");
        for (int j = 0; j < i; ++j)
        {
            to_sort.back() += 'a';
        }
    }
    for (int i = 0; i < limit; ++i)
    {
        to_sort.push_back(to_sort[19]);
        for (int j = limit; j > i; --j)
        {
            to_sort.back() += 'b';
        }
    }
    test_default_ska_sort_combinations(to_sort);
}

int test_main(int, char **)
{
    test_string();
    test_u16string();
    test_u32string();
    test_wstring();
    test_string_unicode();
    test_string_unicode_direct_compare();
    test_string_bad_case();

    return 0;
}


