#pragma once

#ifndef BOOST_SKA_SORT_TEST_COMMON_HPP
#define BOOST_SKA_SORT_TEST_COMMON_HPP

#include <boost/sort/ska_sort/ska_sort_base.hpp>
#include <boost/test/test_tools.hpp>

struct american_flag_sort_settings : boost::sort::detail_ska_sort::default_sort_settings
{
    template<typename>
    static constexpr std::ptrdiff_t insertion_sort_upper_limit = 1;
    static constexpr std::ptrdiff_t american_flag_sort_upper_limit = std::numeric_limits<std::ptrdiff_t>::max();
};
struct ska_byte_sort_settings : boost::sort::detail_ska_sort::default_sort_settings
{
    template<typename>
    static constexpr std::ptrdiff_t insertion_sort_upper_limit = 1;
    static constexpr std::ptrdiff_t american_flag_sort_upper_limit = 1;
};

template<typename T, typename ExtractKey>
struct RememberOriginalPositions
{
    T & container;
    std::vector<size_t> permutation;
    ExtractKey extract_key;

    RememberOriginalPositions(T & container, ExtractKey & extract_key)
        : container(container), extract_key(extract_key)
    {
        std::vector<std::pair<typename T::value_type, size_t>> get_sorted_permutation;
        get_sorted_permutation.reserve(container.size());
        for (size_t i = 0, end = container.size(); i < end; ++i)
        {
            get_sorted_permutation.emplace_back(std::move(container[i]), i);
        }
        permutation.reserve(container.size());
        container.clear();
        auto invoke_wrapper = [extract_key](auto && a) -> decltype(auto)
        {
            return std::invoke(extract_key, std::forward<decltype(a)>(a));
        };
        std::sort(get_sorted_permutation.begin(), get_sorted_permutation.end(), [invoke_wrapper](auto && l, auto && r)
        {
            return invoke_wrapper(l.first) < invoke_wrapper(r.first);
        });
        for (auto & pair : get_sorted_permutation)
        {
            container.push_back(std::move(pair.first));
            permutation.push_back(pair.second);
        }
    }

    void restore_original_positions() const
    {
        using std::swap;
        std::vector<size_t> permutation_copy = permutation;
        for (size_t i = 0, end = container.size() - 1; i < end;)
        {
            size_t desired_position = permutation_copy[i];
            if (desired_position == i)
                ++i;
            else
            {
                swap(container[i], container[desired_position]);
                swap(permutation_copy[i], permutation_copy[desired_position]);
            }
        }
    }

    bool is_sorted() const
    {
        auto invoke_wrapper = [&](auto && a) -> decltype(auto)
        {
            return std::invoke(extract_key, std::forward<decltype(a)>(a));
        };
        return std::is_sorted(container.begin(), container.end(), [invoke_wrapper](auto && l, auto && r)
        {
            return invoke_wrapper(l) < invoke_wrapper(r);
        });
    }
};

template<typename T, typename ExtractKey>
void test_default_ska_sort_combinations(T & container, ExtractKey && extract_key)
{
    RememberOriginalPositions<T, std::remove_reference_t<ExtractKey>> helper(container, extract_key);
    helper.restore_original_positions();
    boost::sort::ska_sort(container.begin(), container.end(), extract_key);
    BOOST_CHECK_MESSAGE(helper.is_sorted(), "ska_sort didn't sort");

    helper.restore_original_positions();
    boost::sort::detail_ska_sort::ska_sort_with_settings<american_flag_sort_settings>(container.begin(), container.end(), extract_key);
    BOOST_CHECK_MESSAGE(helper.is_sorted(), "ska_sort american flag sort didn't sort");

    helper.restore_original_positions();
    boost::sort::detail_ska_sort::ska_sort_with_settings<ska_byte_sort_settings>(container.begin(), container.end(), extract_key);
    BOOST_CHECK_MESSAGE(helper.is_sorted(), "ska_sort byte sort didn't sort");

    helper.restore_original_positions();
}
template<typename T>
void test_default_ska_sort_combinations(T & container)
{
    test_default_ska_sort_combinations(container, boost::sort::detail_ska_sort::iterator_identity_function<typename T::iterator>());
}

template<typename T, typename ExtractKey>
void test_small_key_ska_sort_combinations(T & container, ExtractKey && extract_key)
{
    RememberOriginalPositions<T, std::remove_reference_t<ExtractKey>> helper(container, extract_key);
    helper.restore_original_positions();
    boost::sort::ska_sort_small_key_large_value(container.begin(), container.end(), extract_key);
    BOOST_CHECK_MESSAGE(helper.is_sorted(), "ska_sort_small_key_large_value didn't sort");

    helper.restore_original_positions();
    boost::sort::detail_ska_sort::ska_sort_with_settings_small_key_large_value<american_flag_sort_settings>(container.begin(), container.end(), extract_key);
    BOOST_CHECK_MESSAGE(helper.is_sorted(), "ska_sort_small_key_large_value american flag sort didn't sort");

    helper.restore_original_positions();
    boost::sort::detail_ska_sort::ska_sort_with_settings_small_key_large_value<ska_byte_sort_settings>(container.begin(), container.end(), extract_key);
    BOOST_CHECK_MESSAGE(helper.is_sorted(), "ska_sort_small_key_large_value byte sort didn't sort");

    helper.restore_original_positions();
}
template<typename T>
void test_small_key_ska_sort_combinations(T & container)
{
    test_small_key_ska_sort_combinations(container, boost::sort::detail_ska_sort::iterator_identity_function<typename T::iterator>());
}


#endif
