#pragma once

#ifndef BOOST_SKA_SORT_BASE_HPP
#define BOOST_SKA_SORT_BASE_HPP

#include <boost/config.hpp>
#include <functional>
#include <cstring>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <boost/sort/ska_sort/detail/fixed_size_bit_set.hpp>
#include <boost/sort/ska_sort/detail/algorithm.hpp>
#include <boost/assert.hpp>
#include <boost/core/ignore_unused.hpp>

namespace boost::sort
{

template<typename It>
void ska_sort(It begin, It end);
template<typename It, typename ExtractKey>
void ska_sort(It begin, It end, ExtractKey && extract_key);
template<typename It, typename ExtractKey>
void ska_sort_small_key_large_value(It begin, It end, ExtractKey && extract_key);

template<typename T>
struct ska_sorter;

template<typename T>
struct ska_sort_container_access;
template<typename T>
struct ska_sort_container_compare;

namespace detail_ska_sort
{

// these structs serve two purposes
// 1. they serve as illustration for how to implement the to_radix_sort_key function
// 2. they help produce better error messages. with these overloads you get the
//    error message "no matching function for call to to_radix_sort(your_type)"
//    without these examples, you'd get the error message "to_radix_sort_key was
//    not declared in this scope" which is a much less useful error message
struct example_struct_a { int i; };
struct example_struct_b { float f; };
inline int to_radix_sort_key(example_struct_a a) { return a.i; }
inline float to_radix_sort_key(example_struct_b b) { return b.f; }


template<typename SortSettings, typename It, typename Compare>
BOOST_FORCEINLINE bool insertion_sort_if_less_than_threshold(It begin, It end, std::ptrdiff_t num_elements, Compare && compare)
{
    static constexpr std::ptrdiff_t upper_limit = SortSettings::template insertion_sort_upper_limit<typename std::iterator_traits<It>::value_type>;
    if (num_elements <= 1)
        return true;
    if (num_elements >= upper_limit)
        return false;
    insertion_sort_length_2_or_more(begin, end, compare);
    return true;
}

struct default_sort_settings
{
    static constexpr std::ptrdiff_t insertion_sort_upper_limit_for_size(size_t type_size)
    {
        if (type_size < 16)
            return 32;
        else if (type_size < 24)
            return 24;
        else
            return 16;
    }

    template<typename T>
    static constexpr std::ptrdiff_t insertion_sort_upper_limit = insertion_sort_upper_limit_for_size(sizeof(T));
    static constexpr std::ptrdiff_t american_flag_sort_upper_limit = 2048;
    static constexpr size_t first_loop_unroll_amount = 4;
    static constexpr size_t second_loop_unroll_amount = 4;
    using count_type = size_t;
};

template<typename SortSettings, bool Backwards, typename It, typename FallbackCompare, typename CurrentExtractKey, bool HasNextSortUp>
class ska_sort_impl
{
    template<typename, bool, typename, typename, typename, bool>
    friend class ska_sort_impl;
    template<typename, typename T, typename E>
    friend void ska_sort_with_settings(T, T, E &&);
    template<typename, typename T, typename E>
    friend void ska_sort_with_settings_small_key_large_value(T, T, E &&);

    It stored_begin;
    It stored_end;
    FallbackCompare fallback_compare;
    CurrentExtractKey current_extract_key;

    bool correctly_sorted_or_skipped = false;

    void (*next_sort_up)(It begin, It end, void * up_sort_impl) = nullptr;
    void * up_sort_impl = nullptr;

    using SortFunctionType = void (*)(ska_sort_impl &);
    SortFunctionType sort_next_from_callback = nullptr;
    void * sort_next_from_callback_function = nullptr;
    void * sort_next_from_callback_data = nullptr;

    template<bool HasSortNext, typename ExtractByte>
    void byte_sort(It begin, std::ptrdiff_t num_elements, ExtractByte & extract_byte)
    {
        if (num_elements < SortSettings::american_flag_sort_upper_limit)
        {
            if (num_elements <= std::numeric_limits<uint8_t>::max())
                american_flag_sort<HasSortNext, uint8_t>(begin, num_elements, extract_byte);
            else
                american_flag_sort<HasSortNext, typename SortSettings::count_type>(begin, num_elements, extract_byte);
        }
        else
            ska_byte_sort<HasSortNext, typename SortSettings::count_type>(begin, num_elements, extract_byte);
    }

    template<bool HasSortNext>
    BOOST_FORCEINLINE void recurse_after_sort(It begin, It end, std::ptrdiff_t num_elements)
    {
        if constexpr (Backwards)
        {
            bool did_insertion_sort = insertion_sort_if_less_than_threshold<SortSettings>(begin, end, num_elements, [fallback_compare = fallback_compare](auto && l, auto && r)
            {
                return fallback_compare(r, l);
            });
            if (did_insertion_sort)
                return;
        }
        else
        {
            if (insertion_sort_if_less_than_threshold<SortSettings>(begin, end, num_elements, fallback_compare))
                return;
        }

        static_assert(HasSortNext || HasNextSortUp);
        if constexpr (HasSortNext)
        {
            correctly_sorted_or_skipped = false;
            this->stored_begin = begin;
            this->stored_end = end;
            sort_next_from_callback(*this);
            BOOST_ASSERT(correctly_sorted_or_skipped);
        }
        else
        {
            next_sort_up(begin, end, up_sort_impl);
        }
    }

    template<bool HasSortNext, typename count_type, typename ExtractByte>
    BOOST_NOINLINE void american_flag_sort(It begin, std::ptrdiff_t num_elements, ExtractByte extract_byte)
    {
        count_type counts_and_offsets[256] = {};
        fixed_size_bit_set<256> partitions_used;
        count_type partition_ends[256];
        unroll_loop_nonempty<SortSettings::first_loop_unroll_amount>(begin, num_elements, [extract_byte, counts_and_offsets = &*counts_and_offsets, &partitions_used](It it)
        {
            uint8_t index = extract_byte(*it);
            ++counts_and_offsets[index];
            partitions_used.set_bit(index);
        });
        count_type total = 0;
        uint8_t remaining_partitions[256];
        int num_partitions = 0;
        partitions_used.for_each_set_bit([&](int i)
        {
            count_type count = std::exchange(counts_and_offsets[i], total);
            total += count;
            partition_ends[i] = total;
            remaining_partitions[num_partitions] = i;
            ++num_partitions;
        });
        if (num_partitions > 1)
        {
            uint8_t * current_block_ptr = remaining_partitions;
            uint8_t current_block = *current_block_ptr;
            uint8_t * second_to_last_block = remaining_partitions + num_partitions - 2;
            It it = begin;
            It block_end = begin + partition_ends[current_block];
            for (;;)
            {
                for (;;)
                {
                    uint8_t destination = extract_byte(*it);
                    count_type destination_index = counts_and_offsets[destination]++;
                    if (destination == current_block)
                        break;
                    auto tmp = std::move(begin[destination_index]);
                    destination = extract_byte(tmp);
                    begin[destination_index] = std::move(*it);
                    destination_index = counts_and_offsets[destination]++;
                    if (destination == current_block)
                    {
                        begin[destination_index] = std::move(tmp);
                        break;
                    }
                    *it = std::move(begin[destination_index]);
                    begin[destination_index] = std::move(tmp);
                }
                ++it;
                if (it != block_end)
                    continue;
                for (;;)
                {
                    if (current_block_ptr == second_to_last_block)
                        goto recurse;
                    ++current_block_ptr;
                    current_block = *current_block_ptr;
                    count_type current_block_start = counts_and_offsets[current_block];
                    count_type current_block_end = partition_ends[*current_block_ptr];
                    if (current_block_start != current_block_end)
                    {
                        it = begin + current_block_start;
                        block_end = begin + current_block_end;
                        break;
                    }
                }
            }
        }
        recurse:
        if constexpr (HasSortNext || HasNextSortUp)
        {
            count_type start_offset = 0;
            It partition_begin = begin;
            for (uint8_t * it = remaining_partitions, * remaining_end = remaining_partitions + num_partitions;;)
            {
                count_type end_offset = partition_ends[*it];
                It partition_end = begin + end_offset;
                recurse_after_sort<HasSortNext>(partition_begin, partition_end, end_offset - start_offset);
                start_offset = end_offset;
                partition_begin = partition_end;
                ++it;
                if (it == remaining_end)
                    break;
            }
        }
    }

    template<bool HasSortNext, typename count_type, typename ExtractByte>
    BOOST_NOINLINE void ska_byte_sort(It begin, std::ptrdiff_t num_elements, ExtractByte extract_byte)
    {
        count_type counts_and_offsets[256] = {};
        fixed_size_bit_set<256> partitions_used;
        count_type partition_ends_plus_one[257];
        partition_ends_plus_one[0] = 0;
        count_type * const partition_ends = partition_ends_plus_one + 1;
        unroll_loop_nonempty<SortSettings::first_loop_unroll_amount>(begin, num_elements, [extract_byte, counts_and_offsets = &*counts_and_offsets, &partitions_used](It it)
        {
            uint8_t index = extract_byte(*it);
            ++counts_and_offsets[index];
            partitions_used.set_bit(index);
        });
        uint8_t remaining_partitions[256];
        count_type total = 0;
        int num_partitions = 0;
        partitions_used.for_each_set_bit([&](int i)
        {
            count_type * count_and_offset = counts_and_offsets + i;
            count_type * partition_end = partition_ends + i;
            count_type count = *count_and_offset;
            *count_and_offset = total;
            partition_end[-1] = total;
            total += count;
            *partition_end = total;
            remaining_partitions[num_partitions] = i;
            ++num_partitions;
        });
        for (uint8_t * last_remaining = remaining_partitions + num_partitions, * end_partition = remaining_partitions + 1; last_remaining > end_partition;)
        {
            last_remaining = std_partition_non_empty(remaining_partitions, last_remaining, [&](uint8_t partition)
            {
                count_type & begin_offset = counts_and_offsets[partition];
                count_type end_offset = partition_ends[partition];
                for (;;)
                {
                    if (begin_offset == end_offset)
                        return false;
                    if (extract_byte(begin[begin_offset]) != partition)
                        break;
                    ++begin_offset;
                }

                unroll_loop_nonempty<SortSettings::second_loop_unroll_amount>(begin + begin_offset, end_offset - begin_offset, [begin, counts_and_offsets = &*counts_and_offsets, extract_byte](It it)
                {
                    uint8_t target_partition = extract_byte(*it);
                    count_type offset = counts_and_offsets[target_partition]++;
                    std::iter_swap(it, begin + offset);
                });
                return begin_offset != end_offset;
            });
        }
        if constexpr (HasSortNext || HasNextSortUp)
        {
            for (uint8_t * it = remaining_partitions + num_partitions;;)
            {
                --it;
                uint8_t partition = *it;
                count_type start_offset = partition_ends[partition - 1];
                count_type end_offset = partition_ends[partition];
                recurse_after_sort<HasSortNext>(begin + start_offset, begin + end_offset, end_offset - start_offset);
                if (it == remaining_partitions)
                    break;
            }
        }
    }

    template<bool HasSortNext, typename ExtractBool>
    BOOST_NOINLINE void bool_sort(It begin, It end, ExtractBool && extract_bool)
    {
        It middle = std::partition(begin, end, [&](auto && a)
        {
            return !extract_bool(a);
        });
        if constexpr (HasSortNext || HasNextSortUp)
        {
            recurse_after_sort<HasSortNext>(begin, middle, middle - begin);
            recurse_after_sort<HasSortNext>(middle, end, end - middle);
        }
        else
            boost::ignore_unused(middle);
    }

    static void sort_from_recurse(It begin, It end, void * void_self)
    {
        const ska_sort_impl * self = static_cast<ska_sort_impl *>(void_self);
        ska_sort_impl recurse(*self);
        recurse.correctly_sorted_or_skipped = false;
        recurse.stored_begin = begin;
        recurse.stored_end = end;
        recurse.sort_next_from_callback(recurse);
        BOOST_ASSERT(recurse.correctly_sorted_or_skipped);
    }
    static void sort_from_recurse_reverse(std::reverse_iterator<It> begin, std::reverse_iterator<It> end, void * void_self)
    {
        const ska_sort_impl * self = static_cast<ska_sort_impl *>(void_self);
        ska_sort_impl recurse(*self);
        recurse.correctly_sorted_or_skipped = false;
        recurse.stored_begin = end.base();
        recurse.stored_end = begin.base();
        recurse.sort_next_from_callback(recurse);
        BOOST_ASSERT(recurse.correctly_sorted_or_skipped);
    }
    static void sort_up_reverse(std::reverse_iterator<It> begin, std::reverse_iterator<It> end, void * void_self)
    {
        const ska_sort_impl * self = static_cast<ska_sort_impl *>(void_self);
        ska_sort_impl recurse(*self);
        recurse.next_sort_up(end.base(), begin.base(), recurse.up_sort_impl);
    }
    template<typename Argument>
    static void sort_next_from_callback_with_argument(ska_sort_impl & self)
    {
        void (*next_sort)(ska_sort_impl &, Argument) = reinterpret_cast<void (*)(ska_sort_impl &, Argument)>(self.sort_next_from_callback_function);
        next_sort(self, *static_cast<Argument *>(self.sort_next_from_callback_data));
    }

    template<bool HasSortNext, typename NextKey>
    void sort_with_key(NextKey && next_key)
    {
        BOOST_ASSERT(HasSortNext == (sort_next_from_callback != nullptr));
        auto next_key_combined = [next_key, current_extract_key = current_extract_key](auto && item) -> decltype(auto)
        {
            return next_key(current_extract_key(item));
        };
        using NextKeyType = decltype(next_key_combined(*stored_begin));
        if constexpr (std::is_same_v<unsigned char, NextKeyType>)
        {
            byte_sort<HasSortNext>(stored_begin, stored_end - stored_begin, next_key_combined);
        }
        else if constexpr (std::is_same_v<bool, NextKeyType>)
        {
            bool_sort<HasSortNext>(stored_begin, stored_end, next_key_combined);
        }
        else
        {
            BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
            ska_sort_impl<SortSettings, Backwards, It, FallbackCompare, decltype(next_key_combined), HasSortNext || HasNextSortUp> recurse_sorter{stored_begin, stored_end, fallback_compare, next_key_combined};
            if constexpr (HasSortNext)
            {
                recurse_sorter.next_sort_up = &sort_from_recurse;
                recurse_sorter.up_sort_impl = this;
            }
            else if constexpr (HasNextSortUp)
            {
                recurse_sorter.next_sort_up = next_sort_up;
                recurse_sorter.up_sort_impl = up_sort_impl;
            }
            ska_sorter<NextKeyType>()(recurse_sorter);
            BOOST_ASSERT(recurse_sorter.correctly_sorted_or_skipped);
        }
    }

    template<bool HasSortNext, typename NextKey>
    void sort_with_key_reverse(NextKey && next_key)
    {
        BOOST_ASSERT(HasSortNext == (sort_next_from_callback != nullptr));
        auto next_key_combined = [next_key, current_extract_key = current_extract_key](auto && item) -> decltype(auto)
        {
            return next_key(current_extract_key(item));
        };
        std::reverse_iterator<It> reverse_begin(stored_end);
        std::reverse_iterator<It> reverse_end(stored_begin);
        using NextKeyType = decltype(next_key_combined(*stored_begin));
        ska_sort_impl<SortSettings, !Backwards, std::reverse_iterator<It>, FallbackCompare, decltype(next_key_combined), HasSortNext || HasNextSortUp> recurse_sorter{reverse_begin, reverse_end, fallback_compare, next_key_combined};
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasSortNext)
        {
            recurse_sorter.next_sort_up = &sort_from_recurse_reverse;
            recurse_sorter.up_sort_impl = this;
        }
        else if constexpr (HasNextSortUp)
        {
            recurse_sorter.next_sort_up = &sort_up_reverse;
            recurse_sorter.up_sort_impl = this;
        }
        ska_sorter<NextKeyType>()(recurse_sorter);
        BOOST_ASSERT(recurse_sorter.correctly_sorted_or_skipped);
    }


public:

    ska_sort_impl(It begin, It end, FallbackCompare fallback_compare, CurrentExtractKey current_extract_key)
        : stored_begin(begin), stored_end(end), fallback_compare(std::move(fallback_compare)), current_extract_key(std::move(current_extract_key))
    {
    }

    using SortSettingsType = SortSettings;

    template<typename NextKey, typename NextSortArg>
    void sort(NextKey && next_key, void (*next_sort)(ska_sort_impl &, NextSortArg), NextSortArg next_sort_arg)
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, &sort_next_from_callback_with_argument<NextSortArg>);
        void * old_function = std::exchange(sort_next_from_callback_function, reinterpret_cast<void *>(next_sort));
        void * old_data = std::exchange(sort_next_from_callback_data, std::addressof(next_sort_arg));
        sort_with_key<true>(std::forward<NextKey>(next_key));
        sort_next_from_callback = old_next;
        sort_next_from_callback_function = old_function;
        sort_next_from_callback_data = old_data;
    }
    template<typename NextKey>
    void sort(NextKey && next_key, void (*next_sort)(ska_sort_impl &))
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, next_sort);
        sort_with_key<true>(std::forward<NextKey>(next_key));
        sort_next_from_callback = old_next;
    }
    template<typename NextKey>
    void sort(NextKey && next_key)
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, nullptr);
        sort_with_key<false>(std::forward<NextKey>(next_key));
        sort_next_from_callback = old_next;
    }
    template<typename NextKey, typename Compare, typename NextSortArg>
    void sort_with_faster_comparison(NextKey && next_key, Compare comparison, void (*next_sort)(ska_sort_impl &, NextSortArg), NextSortArg next_sort_arg)
    {
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            // the current_extract_key isn't the last item in the chain
            // for example we might be sorting a std::pair<std::string, bool>
            // and we are hitting the fallback on the string case. in this
            // case we can't use the custom comparison, because that wouldn't
            // take into account the bool. so we have to use the normal fallback
            sort(std::forward<NextKey>(next_key), next_sort, next_sort_arg);
        }
        else
        {
            BOOST_ASSERT(!correctly_sorted_or_skipped);
            correctly_sorted_or_skipped = true;
            auto faster_compare = [comparison, current_extract_key = current_extract_key](auto && l, auto && r)
            {
                return comparison(current_extract_key(l), current_extract_key(r));
            };
            void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, &sort_next_from_callback_with_argument<NextSortArg>);
            void * old_function = std::exchange(sort_next_from_callback_function, reinterpret_cast<void *>(next_sort));
            void * old_data = std::exchange(sort_next_from_callback_data, std::addressof(next_sort_arg));
            ska_sort_impl<SortSettings, Backwards, It, decltype(faster_compare), CurrentExtractKey, true> custom_compare_sorter{stored_begin, stored_end, std::move(faster_compare), current_extract_key};
            custom_compare_sorter.next_sort_up = &sort_from_recurse;
            custom_compare_sorter.up_sort_impl = this;
            custom_compare_sorter.sort(std::forward<NextKey>(next_key));
            sort_next_from_callback = old_next;
            sort_next_from_callback_function = old_function;
            sort_next_from_callback_data = old_data;
        }
    }
    template<typename NextKey, typename Compare>
    void sort_with_faster_comparison(NextKey && next_key, Compare comparison, void (*next_sort)(ska_sort_impl &))
    {
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            // the current_extract_key isn't the last item in the chain
            // for example we might be sorting a std::pair<std::string, bool>
            // and we are hitting the fallback on the string case. in this
            // case we can't use the custom comparison, because that wouldn't
            // take into account the bool. so we have to use the normal fallback
            sort(std::forward<NextKey>(next_key), next_sort);
        }
        else
        {
            BOOST_ASSERT(!correctly_sorted_or_skipped);
            correctly_sorted_or_skipped = true;
            auto faster_compare = [comparison, current_extract_key = current_extract_key](auto && l, auto && r)
            {
                return comparison(current_extract_key(l), current_extract_key(r));
            };
            void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, next_sort);
            ska_sort_impl<SortSettings, Backwards, It, decltype(faster_compare), CurrentExtractKey, true> custom_compare_sorter{stored_begin, stored_end, std::move(faster_compare), current_extract_key};
            custom_compare_sorter.next_sort_up = &sort_from_recurse;
            custom_compare_sorter.up_sort_impl = this;
            custom_compare_sorter.sort(std::forward<NextKey>(next_key));
            sort_next_from_callback = old_next;
        }
    }
    template<typename NextKey, typename Compare>
    void sort_with_faster_comparison(NextKey && next_key, Compare comparison)
    {
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            // the current_extract_key isn't the last item in the chain
            // for example we might be sorting a std::pair<std::string, bool>
            // and we are hitting the fallback on the string case. in this
            // case we can't use the custom comparison, because that wouldn't
            // take into account the bool. so we have to use the normal fallback
            sort(std::forward<NextKey>(next_key));
        }
        else
        {
            BOOST_ASSERT(!correctly_sorted_or_skipped);
            correctly_sorted_or_skipped = true;
            auto faster_compare = [comparison, current_extract_key = current_extract_key](auto && l, auto && r)
            {
                return comparison(current_extract_key(l), current_extract_key(r));
            };
            ska_sort_impl<SortSettings, Backwards, It, decltype(faster_compare), CurrentExtractKey, false> custom_compare_sorter{stored_begin, stored_end, std::move(faster_compare), current_extract_key};
            custom_compare_sorter.sort(std::forward<NextKey>(next_key));
        }
    }

    template<typename NextKey, typename NextSortArg>
    void sort_backwards(NextKey && next_key, void (*next_sort)(ska_sort_impl &, NextSortArg), NextSortArg next_sort_arg)
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, &sort_next_from_callback_with_argument<NextSortArg>);
        void * old_function = std::exchange(sort_next_from_callback_function, reinterpret_cast<void *>(next_sort));
        void * old_data = std::exchange(sort_next_from_callback_data, std::addressof(next_sort_arg));
        sort_with_key_reverse<true>(std::forward<NextKey>(next_key));
        sort_next_from_callback = old_next;
        sort_next_from_callback_function = old_function;
        sort_next_from_callback_data = old_data;
    }
    template<typename NextKey>
    void sort_backwards(NextKey && next_key, void (*next_sort)(ska_sort_impl &))
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, next_sort);
        sort_with_key_reverse<true>(std::forward<NextKey>(next_key));
        sort_next_from_callback = old_next;
    }
    template<typename NextKey>
    void sort_backwards(NextKey && next_key)
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, nullptr);
        sort_with_key_reverse<false>(std::forward<NextKey>(next_key));
        sort_next_from_callback = old_next;
    }
    template<typename NextKey, typename Compare, typename NextSortArg>
    void sort_backwards_with_faster_comparison(NextKey && next_key, Compare comparison, void (*next_sort)(ska_sort_impl &, NextSortArg), NextSortArg next_sort_arg)
    {
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            // the current_extract_key isn't the last item in the chain
            // for example we might be sorting a std::pair<std::string, bool>
            // and we are hitting the fallback on the string case. in this
            // case we can't use the custom comparison, because that wouldn't
            // take into account the bool. so we have to use the normal fallback
            sort_backwards(std::forward<NextKey>(next_key), next_sort, next_sort_arg);
        }
        else
        {
            BOOST_ASSERT(!correctly_sorted_or_skipped);
            correctly_sorted_or_skipped = true;
            auto faster_compare = [comparison, current_extract_key = current_extract_key](auto && l, auto && r)
            {
                return comparison(current_extract_key(l), current_extract_key(r));
            };
            void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, &sort_next_from_callback_with_argument<NextSortArg>);
            void * old_function = std::exchange(sort_next_from_callback_function, reinterpret_cast<void *>(next_sort));
            void * old_data = std::exchange(sort_next_from_callback_data, std::addressof(next_sort_arg));
            ska_sort_impl<SortSettings, Backwards, It, decltype(faster_compare), CurrentExtractKey, true> custom_compare_sorter{stored_begin, stored_end, std::move(faster_compare), current_extract_key};
            custom_compare_sorter.next_sort_up = &sort_from_recurse;
            custom_compare_sorter.up_sort_impl = this;
            custom_compare_sorter.sort_backwards(std::forward<NextKey>(next_key));
            sort_next_from_callback = old_next;
            sort_next_from_callback_function = old_function;
            sort_next_from_callback_data = old_data;
        }
    }
    template<typename NextKey, typename Compare>
    void sort_backwards_with_faster_comparison(NextKey && next_key, Compare comparison, void (*next_sort)(ska_sort_impl &))
    {
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            // the current_extract_key isn't the last item in the chain
            // for example we might be sorting a std::pair<std::string, bool>
            // and we are hitting the fallback on the string case. in this
            // case we can't use the custom comparison, because that wouldn't
            // take into account the bool. so we have to use the normal fallback
            sort_backwards(std::forward<NextKey>(next_key), next_sort);
        }
        else
        {
            BOOST_ASSERT(!correctly_sorted_or_skipped);
            correctly_sorted_or_skipped = true;
            auto faster_compare = [comparison, current_extract_key = current_extract_key](auto && l, auto && r)
            {
                return comparison(current_extract_key(l), current_extract_key(r));
            };
            void (*old_next)(ska_sort_impl &) = std::exchange(sort_next_from_callback, next_sort);
            ska_sort_impl<SortSettings, Backwards, It, decltype(faster_compare), CurrentExtractKey, true> custom_compare_sorter{stored_begin, stored_end, std::move(faster_compare), current_extract_key};
            custom_compare_sorter.next_sort_up = &sort_from_recurse;
            custom_compare_sorter.up_sort_impl = this;
            custom_compare_sorter.sort_backwards(std::forward<NextKey>(next_key));
            sort_next_from_callback = old_next;
        }
    }
    template<typename NextKey, typename Compare>
    void sort_backwards_with_faster_comparison(NextKey && next_key, Compare comparison)
    {
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            // the current_extract_key isn't the last item in the chain
            // for example we might be sorting a std::pair<std::string, bool>
            // and we are hitting the fallback on the string case. in this
            // case we can't use the custom comparison, because that wouldn't
            // take into account the bool. so we have to use the normal fallback
            sort_backwards(std::forward<NextKey>(next_key));
        }
        else
        {
            BOOST_ASSERT(!correctly_sorted_or_skipped);
            correctly_sorted_or_skipped = true;
            auto faster_compare = [comparison, current_extract_key = current_extract_key](auto && l, auto && r)
            {
                return comparison(current_extract_key(l), current_extract_key(r));
            };
            ska_sort_impl<SortSettings, Backwards, It, decltype(faster_compare), CurrentExtractKey, false> custom_compare_sorter{stored_begin, stored_end, std::move(faster_compare), current_extract_key};
            custom_compare_sorter.sort_backwards(std::forward<NextKey>(next_key));
        }
    }

    decltype(auto) first_item() const
    {
        return current_extract_key(*stored_begin);
    }
    template<typename Callback>
    void for_each_item(Callback && callback) const
    {
        for (It it = stored_begin, end = stored_end; it != end; ++it)
        {
            if (!callback(current_extract_key(*it)))
                break;
        }
    }

    void skip()
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
            next_sort_up(stored_begin, stored_end, up_sort_impl);
    }
    void std_sort_fallback()
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        std::sort(stored_begin, stored_end, fallback_compare);
    }
    template<typename CustomCompare>
    void std_sort_fallback(CustomCompare && compare)
    {
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            // the current_extract_key isn't the last item in the chain
            // for example we might be sorting a std::pair<std::string, bool>
            // and we are hitting the fallback on the string case. in this
            // case we can't use the custom comparison, because that wouldn't
            // take into account the bool. so we have to use the normal fallback
            std_sort_fallback();
        }
        else
        {
            BOOST_ASSERT(!correctly_sorted_or_skipped);
            correctly_sorted_or_skipped = true;
            std::sort(stored_begin, stored_end, [compare, current_extract_key = current_extract_key](auto && l, auto && r)
            {
                if constexpr (Backwards)
                {
                    return compare(current_extract_key(r), current_extract_key(l));
                }
                else
                {
                    return compare(current_extract_key(l), current_extract_key(r));
                }
            });
        }
    }

    template<typename ProxyType, typename Sorter, typename Compare = std::less<>>
    void sort_with_proxy(Compare compare = Compare())
    {
        BOOST_ASSERT(!correctly_sorted_or_skipped);
        correctly_sorted_or_skipped = true;
        std::vector<std::pair<ProxyType, size_t>> sort_this_instead;
        size_t num_items = stored_end - stored_begin;
        sort_this_instead.reserve(num_items);
        for_each_item([index = size_t(), &sort_this_instead](const auto & to_sort) mutable
        {
            sort_this_instead.emplace_back(to_sort, index);
            ++index;
            return true;
        });
        auto extract_first = [](std::pair<ProxyType, size_t> & pair) -> ProxyType &
        {
            return pair.first;
        };
        using iterator_type = typename std::vector<std::pair<ProxyType, size_t>>::iterator;
        std::vector<std::pair<iterator_type, iterator_type>> subsections;
        BOOST_ASSERT(HasNextSortUp == (next_sort_up != nullptr));
        if constexpr (HasNextSortUp)
        {
            auto new_compare = [stored_begin = stored_begin, fallback_compare = fallback_compare](std::pair<ProxyType, size_t> & l, std::pair<ProxyType, size_t> & r)
            {
                return fallback_compare(stored_begin[l.second], stored_begin[r.second]);
            };
            ska_sort_impl<SortSettings, false, decltype(sort_this_instead.begin()), decltype(new_compare), decltype(extract_first), true> new_sorter{sort_this_instead.begin(), sort_this_instead.end(), new_compare, extract_first};
            subsections.reserve(num_items / 2);
            new_sorter.next_sort_up = [](iterator_type begin, iterator_type end, void * state_ptr)
            {
                std::vector<std::pair<iterator_type, iterator_type>> * subsections = static_cast<std::vector<std::pair<iterator_type, iterator_type>> *>(state_ptr);
                subsections->emplace_back(begin, end);
            };
            new_sorter.up_sort_impl = &subsections;
            Sorter()(new_sorter);
            BOOST_ASSERT(new_sorter.correctly_sorted_or_skipped);
        }
        else
        {
            auto new_compare = [compare](std::pair<ProxyType, size_t> & l, std::pair<ProxyType, size_t> & r)
            {
                return compare(l.first, r.first);
            };
            ska_sort_impl<SortSettings, false, decltype(sort_this_instead.begin()), decltype(new_compare), decltype(extract_first), false> new_sorter{sort_this_instead.begin(), sort_this_instead.end(), new_compare, extract_first};
            Sorter()(new_sorter);
            BOOST_ASSERT(new_sorter.correctly_sorted_or_skipped);
        }

        auto begin = stored_begin;
        for (size_t i = 0, end = sort_this_instead.size(); i < end; ++i)
        {
            size_t original_index = sort_this_instead[i].second;
            if (original_index == i)
                continue;
            auto tmp = std::move(begin[i]);
            size_t target_index = i;
            do
            {
                begin[target_index] = std::move(begin[original_index]);
                target_index = original_index;
                original_index = std::exchange(sort_this_instead[original_index].second, original_index);
            }
            while(original_index != i);
            begin[target_index] = std::move(tmp);
        }
        if constexpr (HasNextSortUp)
        {
            iterator_type sorted_begin = sort_this_instead.begin();
            for (const std::pair<iterator_type, iterator_type> & section_to_sort : subsections)
            {
                size_t begin_index = section_to_sort.first - sorted_begin;
                size_t end_index = section_to_sort.second - sorted_begin;
                next_sort_up(begin + begin_index, begin + end_index, up_sort_impl);
            }
        }
    }
};

struct by_value_to_unsigned_sorter
{
    static unsigned char to_unsigned(signed char c)
    {
        constexpr unsigned char sign_bit = 128;
        return static_cast<unsigned char>(c) ^ sign_bit;
    }
    static unsigned char to_unsigned(char c)
    {
        return static_cast<unsigned char>(c);
    }
    static std::uint16_t to_unsigned(char16_t c)
    {
        return static_cast<std::uint16_t>(c);
    }
    static std::uint32_t to_unsigned(char32_t c)
    {
        return static_cast<std::uint32_t>(c);
    }
    static auto to_unsigned(wchar_t c)
    {
        if constexpr (sizeof(wchar_t) == sizeof(std::uint16_t))
        {
            return static_cast<std::uint16_t>(c);
        }
        else
        {
            static_assert(sizeof(wchar_t) == sizeof(std::uint32_t));
            return static_cast<std::uint32_t>(c);
        }
    }
    static unsigned short to_unsigned(short i)
    {
        constexpr unsigned short sign_bit = static_cast<unsigned short>(1 << (sizeof(short) * 8 - 1));
        return static_cast<unsigned short>(i) ^ sign_bit;
    }
    static unsigned int to_unsigned(int i)
    {
        constexpr unsigned int sign_bit = static_cast<unsigned int>(1 << (sizeof(int) * 8 - 1));
        return static_cast<unsigned int>(i) ^ sign_bit;
    }
    static unsigned long to_unsigned(long l)
    {
        constexpr unsigned long sign_bit = static_cast<unsigned long>(1l << (sizeof(long) * 8 - 1));
        return static_cast<unsigned long>(l) ^ sign_bit;
    }
    static unsigned long long to_unsigned(long long l)
    {
        constexpr unsigned long long sign_bit = static_cast<unsigned long long>(1ll << (sizeof(long long) * 8 - 1));
        return static_cast<unsigned long long>(l) ^ sign_bit;
    }
    template<typename Sorter>
    void operator()(Sorter & sorter) const
    {
        sorter.sort([](auto value)
        {
            return to_unsigned(value);
        });
    }
};
template<size_t Size>
struct sized_unsigned_sorter
{
    template<size_t Index, typename Sorter>
    static void sort_byte(Sorter & sorter)
    {
        if constexpr (Index == 0)
        {
            sorter.sort([](auto value)
            {
                return static_cast<unsigned char>(value & 0xff);
            });
        }
        else
        {
            sorter.sort([](auto value)
            {
                static constexpr size_t ToShift = Index * 8;
                static constexpr decltype(value) mask = static_cast<decltype(value)>(0xff) << ToShift;
                return static_cast<unsigned char>((value & mask) >> ToShift);
            }, &sort_byte<Index - 1>);
        }
    }

    template<typename Sorter>
    void operator()(Sorter & sorter) const
    {
        sort_byte<Size - 1>(sorter);
    }
};

struct compare_float_as_uint
{
    static uint16_t as_uint(float f)
    {
        uint16_t result;
        static_assert(sizeof(float) == sizeof(uint16_t) + 2);
        // don't need the top two bytes. I'll only come in here if we're
        // already done sorting the top two bytes
        std::memcpy(&result, &f, sizeof(result));
        return result;
    }
    static uint64_t as_uint(double d)
    {
        uint64_t result;
        static_assert(sizeof(double) == sizeof(uint64_t));
        std::memcpy(&result, &d, sizeof(result));
        // don't need the top two bytes. I'll only come in here if we're
        // already done sorting the top two bytes
        return result & 0x0000'ffff'ffff'ffff;
    }
    static uint64_t as_uint(long double ld)
    {
        if constexpr (sizeof(long double) == sizeof(double))
        {
            return as_uint(static_cast<double>(ld));
        }
        else
        {
            uint64_t result;
            static_assert(sizeof(long double) > sizeof(uint64_t));
            // don't need the top two bytes. I'll only come in here if we're
            // already done sorting the top two bytes
            std::memcpy(&result, &ld, sizeof(result));
            return result;
        }
    }
    bool operator()(float l, float r) const
    {
        return as_uint(l) < as_uint(r);
    }
    bool operator()(double l, double r) const
    {
        return as_uint(l) < as_uint(r);
    }
    bool operator()(long double l, long double r) const
    {
        return as_uint(l) < as_uint(r);
    }
};

struct compare_float_as_uint_negative : compare_float_as_uint
{
    template<typename F>
    bool operator()(F l, F r) const
    {
        return static_cast<const compare_float_as_uint &>(*this)(r, l);
    }
};

template<typename T, size_t Size = sizeof(T)>
struct float_sorter
{
    static_assert(std::numeric_limits<T>::is_iec559);

    template<size_t Index, typename Sorter>
    static void sort_byte(Sorter & sorter)
    {
        if constexpr (Index == 0)
        {
            sorter.sort_with_faster_comparison([](T value)
            {
                return get_byte<Index>(value);
            }, compare_float_as_uint{});
        }
        else
        {
            sorter.sort_with_faster_comparison([](T value)
            {
                return get_byte<Index>(value);
            }, compare_float_as_uint{}, &sort_byte<Index - 1>);
        }
    }
    template<size_t Index, typename Sorter>
    static void sort_byte_backwards(Sorter & sorter)
    {
        if constexpr (Index == 0)
        {
            sorter.sort_backwards_with_faster_comparison([](T value)
            {
                return get_byte<Index>(value);
            }, compare_float_as_uint_negative{});
        }
        else
        {
            sorter.sort_backwards_with_faster_comparison([](T value)
            {
                return get_byte<Index>(value);
            }, compare_float_as_uint_negative{}, &sort_byte_backwards<Index - 1>);
        }
    }
    template<typename Sorter>
    static void sort_after_sign(Sorter & sorter)
    {
        T first_item = sorter.first_item();
        if (first_item == 0 || !sign_bit(first_item))
        {
            sort_byte<Size - 2>(sorter);
        }
        else
        {
            sort_byte_backwards<Size - 2>(sorter);
        }
    }

    static bool sign_bit(T f)
    {
        return (get_byte<Size - 1>(f) & 0x80) != 0;
    }
    template<size_t Index>
    static std::uint8_t get_byte(T f)
    {
        static_assert(sizeof(T) == Size || (std::is_same_v<T, long double> && sizeof(T) >= Size));
        static_assert(Index < Size);
        return reinterpret_cast<const std::uint8_t *>(std::addressof(f))[Index];
    }
    static uint8_t first_byte(T f)
    {
        if (f == 0)
            return 0x80;
        std::uint8_t byte = get_byte<Size - 1>(f);
        std::uint8_t sign_bit = -std::int8_t(byte >> 7);
        return byte ^ (sign_bit | 0x80);
        // todo: on GCC the below code is faster because it turns this into
        // branchless code as well, using a cmovs instruction. I don't have
        // access to that instruction using C++, so I just have to trust the
        // compiler optimizer. unfortunately clang doesn't use cmovs here and
        // on a randomly shuffled array the code using branches is much slower
        // than the above code.
        // so I'll leave it at my custom branchless code above
        /*if (byte & 0x80)
            return ~byte;
        else
            return byte | 0x80;*/
    }

    template<typename Sorter>
    void operator()(Sorter & sorter)
    {
        sorter.sort([](T f)
        {
            return first_byte(f);
        }, &sort_after_sign<Sorter>);
    }
};



struct default_container_access_member_begin
{
    template<typename T>
    static decltype(auto) begin(const T & container)
    {
        return container.begin();
    }
    template<typename T>
    static decltype(auto) end(const T & container)
    {
        return container.end();
    }
};

struct call_non_member_begin_end
{
    template<typename T>
    static decltype(auto) call_begin(const T & container)
    {
        return begin(container);
    }
    template<typename T>
    static decltype(auto) call_end(const T & container)
    {
        return end(container);
    }
};

struct default_container_access_free_begin
{
    template<typename T>
    static decltype(auto) begin(const T & container)
    {
        return call_non_member_begin_end::call_begin(container);
    }
    template<typename T>
    static decltype(auto) end(const T & container)
    {
        return call_non_member_begin_end::call_end(container);
    }
};

template<typename T, typename Enable = void>
struct container_access_decision_default_impl_member
{
    static constexpr bool use_default_impl = false;
};
template<typename T>
struct container_access_decision_default_impl_member<T, std::void_t<decltype(std::declval<T>().begin())>>
{
    static constexpr bool use_default_impl = true;
};
template<typename T, typename Enable = void>
struct container_access_decision_default_impl_free
{
    static constexpr bool use_default_impl = false;
};
template<typename T>
struct container_access_decision_default_impl_free<T, std::void_t<decltype(begin(std::declval<T>()))>>
{
    static constexpr bool use_default_impl = true;
};
template<typename T, typename Enable = void>
struct container_access_decision_custom_impl
{
    static constexpr bool use_custom_impl = false;
};
template<typename T>
struct container_access_decision_custom_impl<T, std::void_t<decltype(ska_sort_container_access<T>::begin(std::declval<T>()))>>
{
    static constexpr bool use_custom_impl = true;
};

template<typename T>
struct container_access_decision
{
    using type = std::conditional_t<container_access_decision_custom_impl<T>::use_custom_impl, ska_sort_container_access<T>,
                 std::conditional_t<container_access_decision_default_impl_free<T>::use_default_impl, default_container_access_free_begin,
                 std::conditional_t<container_access_decision_default_impl_member<T>::use_default_impl, default_container_access_member_begin,
                 void>>>;
};


template<typename T, typename Enable = void>
struct fallback_ska_sorter
{
    template<typename Sorter>
    void operator()(Sorter & sorter)
    {
        sorter.sort([](auto && v) -> decltype(auto)
        {
            return to_radix_sort_key(v);
        });
    }
};


template<typename It>
struct iterator_identity_function
{
    using value_type = typename std::iterator_traits<It>::value_type;
    const value_type & operator()(const value_type & v) const
    {
        return v;
    }
    value_type & operator()(value_type & v) const
    {
        return v;
    }
    value_type operator()(value_type && v) const
    {
        return std::move(v);
    }
};

template<typename T>
decltype(auto) convert_to_radix_sort_key(T && value);

template<typename T, typename Enable = void>
struct convert_to_radix_sort_key_specialization
{
    static const T & convert(const T & value)
    {
        return value;
    }
    static T convert(T && value)
    {
        return std::move(value);
    }
};
template<typename T>
struct convert_to_radix_sort_key_specialization<T, std::void_t<decltype(to_radix_sort_key(std::declval<T>()))>>
{
    static decltype(auto) convert(const T & value)
    {
        return convert_to_radix_sort_key(to_radix_sort_key(value));
    }
    static decltype(auto) convert(T && value)
    {
        return convert_to_radix_sort_key(to_radix_sort_key(std::move(value)));
    }
};

template<typename T>
decltype(auto) convert_to_radix_sort_key(T && value)
{
    return convert_to_radix_sort_key_specialization<std::decay_t<T>>::convert(std::forward<T>(value));
}

template<typename T>
inline bool ska_sort_equality_compare(const T & l, const T & r)
{
    const auto & l_key = convert_to_radix_sort_key(l);
    const auto & r_key = convert_to_radix_sort_key(r);
    return l_key == r_key;
}

struct list_sort_data
{
    size_t current_index = 0;
    size_t recursion_limit = 16;
};

template<typename T>
struct fallback_ska_sorter<T,
        std::enable_if_t<
            !std::is_same_v<typename container_access_decision<T>::type, void>
         && std::is_convertible_v<
                    typename std::iterator_traits<decltype(container_access_decision<T>::type::begin(std::declval<T>()))>::iterator_category,
                    std::random_access_iterator_tag
                    >
        >
       >
{
    using container_access = typename container_access_decision<T>::type;

    static size_t container_size(const T & list)
    {
        return container_access::end(list) - container_access::begin(list);
    }

    struct sort_at_index
    {
        size_t index = 0;
        bool operator()(const T & l, const T & r) const
        {
            return ska_sort_container_compare<T>::lexicographical_compare(
                        std::next(container_access::begin(l), index),
                        container_access::end(l),
                        std::next(container_access::begin(r), index),
                        container_access::end(r));
        }
    };

    template<typename Sorter>
    static size_t common_prefix(Sorter & sorter, size_t start_index)
    {
        const auto & largest_match_list = sorter.first_item();
        size_t largest_match = container_size(largest_match_list);
        if (largest_match == start_index)
            return start_index;
        sorter.for_each_item([&, largest_match_begin = container_access::begin(largest_match_list), first = true](const auto & current_list) mutable
        {
            if (first)
            {
                first = false;
                return true;
            }
            auto current_begin = container_access::begin(current_list);
            size_t current_size = container_size(current_list);
            if (current_size < largest_match)
            {
                largest_match = current_size;
                if (largest_match == start_index)
                    return false;
            }
            if (!ska_sort_equality_compare(largest_match_begin[start_index], current_begin[start_index]))
            {
                largest_match = start_index;
                return false;
            }
            for (size_t i = start_index + 1; i < largest_match; ++i)
            {
                if (!ska_sort_equality_compare(largest_match_begin[i], current_begin[i]))
                {
                    largest_match = i;
                    break;
                }
            }
            return true;
        });
        return largest_match;
    }

    template<typename Sorter>
    static void sort(Sorter & sorter, list_sort_data sort_data)
    {
        sort_data.current_index = common_prefix(sorter, sort_data.current_index);
        sorter.sort([current_index = sort_data.current_index](const auto & list)
        {
            return container_size(list) > current_index;
        }, static_cast<void (*)(Sorter &, list_sort_data)>([](Sorter & sorter, list_sort_data sort_data)
        {
            if (container_size(sorter.first_item()) <= sort_data.current_index)
                sorter.skip();
            else
            {
                sorter.sort_with_faster_comparison([current_index = sort_data.current_index](const auto & list) -> decltype(auto)
                {
                    return container_access::begin(list)[current_index];
                }, sort_at_index{sort_data.current_index}, &sort_from_recursion<Sorter>, sort_data);
            }
        }), sort_data);
    }

    template<typename Sorter>
    static void sort_from_recursion(Sorter & sorter, list_sort_data sort_data)
    {
        ++sort_data.current_index;
        --sort_data.recursion_limit;
        if (sort_data.recursion_limit == 0)
        {
            sorter.std_sort_fallback(sort_at_index{sort_data.current_index});
        }
        else
        {
            sort(sorter, sort_data);
        }
    }

    template<typename Sorter>
    void operator()(Sorter & sorter)
    {
        sort(sorter, list_sort_data());
    }
};

template<typename T>
struct fallback_ska_sorter<T,
        std::enable_if_t<
            !std::is_same_v<typename container_access_decision<T>::type, void>
         && !std::is_convertible_v<
                typename std::iterator_traits<decltype(container_access_decision<T>::type::begin(std::declval<T>()))>::iterator_category,
                std::random_access_iterator_tag>
         && std::is_convertible_v<
                typename std::iterator_traits<decltype(container_access_decision<T>::type::begin(std::declval<T>()))>::iterator_category,
                std::forward_iterator_tag>
        >
       >
{
    using container_access = typename container_access_decision<T>::type;

    using iterator_type = decltype(container_access::begin(std::declval<T>()));

    struct current_iterator_positions
    {
        current_iterator_positions(const T & to_sort)
            : it(container_access::begin(to_sort))
            , end(container_access::end(to_sort))
        {
        }

        iterator_type it;
        iterator_type end;
    };
    struct iterator_based_fallback
    {
        bool operator()(const current_iterator_positions & l, const current_iterator_positions & r) const
        {
            return ska_sort_container_compare<T>::lexicographical_compare(l.it, l.end, r.it, r.end);
        }
    };

    struct forward_iterator_sorter
    {
        template<typename Sorter>
        static void skip_common_prefix(Sorter & sorter)
        {
            const current_iterator_positions & largest_match_list = sorter.first_item();
            if (largest_match_list.it == largest_match_list.end)
                return;
            size_t largest_match = std::numeric_limits<size_t>::max();
            sorter.for_each_item([&, first = true](const current_iterator_positions & current_list) mutable
            {
                if (first)
                {
                    first = false;
                    return true;
                }
                size_t match_size = 0;
                for (iterator_type it = current_list.it, end = current_list.end, it2 = largest_match_list.it, it2end = largest_match_list.end; it != end; ++it)
                {
                    if (!ska_sort_equality_compare(*it, *it2))
                        break;
                    ++match_size;
                    if (match_size == largest_match)
                        return true;
                    ++it2;
                    if (it2 == it2end)
                        break;
                }
                if (match_size < largest_match)
                {
                    if (match_size == 0)
                    {
                        largest_match = 0;
                        return false;
                    }
                    largest_match = match_size;
                }
                return true;
            });
            if (largest_match == 0)
                return;
            sorter.for_each_item([largest_match](current_iterator_positions & current_list)
            {
                for (size_t i = 0; i < largest_match; ++i)
                    ++current_list.it;
                return true;
            });
        }

        template<typename Sorter>
        static void sort(Sorter & sorter, size_t recursion_limit)
        {
            skip_common_prefix(sorter);
            sorter.sort([](const current_iterator_positions & list)
            {
                return list.it != list.end;
            }, static_cast<void (*)(Sorter &, size_t)>([](Sorter & sorter, size_t recursion_limit)
            {
                const current_iterator_positions & first_item = sorter.first_item();
                if (first_item.it == first_item.end)
                    sorter.skip();
                else
                {
                    sorter.sort([](const current_iterator_positions & proxy) -> decltype(auto)
                    {
                        return *proxy.it;
                    }, &sort_from_recursion<Sorter>, recursion_limit);
                }
            }), recursion_limit);
        }

        template<typename Sorter>
        void operator()(Sorter & sorter)
        {
            sort(sorter, 16);
        }

        template<typename Sorter>
        static void sort_from_recursion(Sorter & sorter, size_t recursion_limit)
        {
            sorter.for_each_item([](current_iterator_positions & proxy)
            {
                ++proxy.it;
                return true;
            });
            --recursion_limit;
            if (recursion_limit == 0)
            {
                sorter.std_sort_fallback(iterator_based_fallback());
            }
            else
            {
                sort(sorter, recursion_limit);
            }
        }

    };

    template<typename Sorter>
    void operator()(Sorter & sorter)
    {
        sorter.template sort_with_proxy<current_iterator_positions, forward_iterator_sorter>(iterator_based_fallback());
    }
};

template<typename SortSettings, typename It, typename ExtractKey>
void ska_sort_with_settings(It begin, It end, ExtractKey && extract_key)
{
    auto invoke_wrapper = [extract_key](auto && a) -> decltype(auto)
    {
        return std::invoke(extract_key, std::forward<decltype(a)>(a));
    };
    std::ptrdiff_t num_elements = end - begin;
    auto fallback_compare = [invoke_wrapper](auto && l, auto && r)
    {
        return invoke_wrapper(l) < invoke_wrapper(r);
    };
    if (insertion_sort_if_less_than_threshold<SortSettings>(begin, end, num_elements, fallback_compare))
        return;
    ska_sort_impl<SortSettings, false, It, decltype(fallback_compare), decltype(invoke_wrapper), false> impl{begin, end, fallback_compare, invoke_wrapper};
    using sorter = ska_sorter<std::decay_t<decltype(invoke_wrapper(*begin))>>;
    sorter()(impl);
    BOOST_ASSERT(impl.correctly_sorted_or_skipped);
}
template<typename SortSettings, typename It>
void ska_sort_with_settings(It begin, It end)
{
    return ska_sort_with_settings<SortSettings>(begin, end, iterator_identity_function<It>());
}

template<typename SortSettings, typename It, typename ExtractKey>
void ska_sort_with_settings_small_key_large_value(It begin, It end, ExtractKey && extract_key)
{
    auto invoke_wrapper = [extract_key](auto && a) -> decltype(auto)
    {
        return std::invoke(extract_key, std::forward<decltype(a)>(a));
    };
    std::ptrdiff_t num_elements = end - begin;
    auto fallback_compare = [invoke_wrapper](auto && l, auto && r)
    {
        return invoke_wrapper(l) < invoke_wrapper(r);
    };
    if (insertion_sort_if_less_than_threshold<SortSettings>(begin, end, num_elements, fallback_compare))
        return;
    ska_sort_impl<SortSettings, false, It, decltype(fallback_compare), decltype(invoke_wrapper), false> impl{begin, end, fallback_compare, invoke_wrapper};
    using key_type = std::decay_t<decltype(invoke_wrapper(*begin))>;
    impl.template sort_with_proxy<key_type, ska_sorter<key_type>>();
    BOOST_ASSERT(impl.correctly_sorted_or_skipped);
}

} // end namespace detail

template<typename T>
struct ska_sorter<T &> : ska_sorter<T>
{
};
template<typename T>
struct ska_sorter<T &&> : ska_sorter<T>
{
};
template<typename T>
struct ska_sorter<const T> : ska_sorter<T>
{
};
template<typename T>
struct ska_sorter<volatile T> : ska_sorter<T>
{
};

template<>
struct ska_sorter<bool>
{
    template<typename Sorter>
    void operator()(Sorter & sorter)
    {
        sorter.sort([](bool value)
        {
            return value;
        });
    }
};

template<> struct ska_sorter<unsigned char> : detail_ska_sort::sized_unsigned_sorter<sizeof(unsigned char)> {};
template<> struct ska_sorter<unsigned short> : detail_ska_sort::sized_unsigned_sorter<sizeof(unsigned short)> {};
template<> struct ska_sorter<unsigned int> : detail_ska_sort::sized_unsigned_sorter<sizeof(unsigned int)> {};
template<> struct ska_sorter<unsigned long> : detail_ska_sort::sized_unsigned_sorter<sizeof(unsigned long)> {};
template<> struct ska_sorter<unsigned long long> : detail_ska_sort::sized_unsigned_sorter<sizeof(unsigned long long)> {};

template<> struct ska_sorter<char> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<signed char> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<char16_t> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<char32_t> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<wchar_t> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<signed short> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<signed int> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<signed long> : detail_ska_sort::by_value_to_unsigned_sorter {};
template<> struct ska_sorter<signed long long> : detail_ska_sort::by_value_to_unsigned_sorter {};

template<> struct ska_sorter<float> : detail_ska_sort::float_sorter<float> {};
template<> struct ska_sorter<double> : detail_ska_sort::float_sorter<double> {};
template<> struct ska_sorter<long double> : detail_ska_sort::float_sorter<long double, sizeof(long double) == sizeof(double) ? sizeof(long double) : 10> {};

template<typename T>
struct ska_sorter<T *>
{
    template<typename Sorter>
    void operator()(Sorter & sorter) const
    {
        return sorter.sort([](T * ptr)
        {
            return reinterpret_cast<size_t>(ptr);
        });
    }
};
template<typename K, typename V>
struct ska_sorter<std::pair<K, V>>
{
    template<typename Sorter>
    static void SortSecond(Sorter & sorter)
    {
        sorter.sort([](const std::pair<K, V> & value) -> const V &
        {
            return value.second;
        });
    }

    template<typename Sorter>
    void operator()(Sorter & sorter)
    {
        sorter.sort([](const std::pair<K, V> & value) -> const K &
        {
            return value.first;
        }, &SortSecond<Sorter>);
    }
};

template<typename T>
struct ska_sort_container_compare
{
    using const_iterator = typename T::const_iterator;
    static bool lexicographical_compare(const_iterator l_it, const_iterator l_end, const_iterator r_it, const_iterator r_end)
    {
        return std::lexicographical_compare(l_it, l_end, r_it, r_end);
    }
};

template<typename C, typename T, typename A>
struct ska_sort_container_compare<std::basic_string<C, T, A>>
{
    using const_iterator = typename std::basic_string<C, T, A>::const_iterator;
    static bool lexicographical_compare(const_iterator l_it, const_iterator l_end, const_iterator r_it, const_iterator r_end)
    {
        size_t l_size = l_end - l_it;
        size_t r_size = r_end - r_it;
        bool l_is_smaller = l_size < r_size;
        int compare_result = T::compare(std::addressof(*l_it), std::addressof(*r_it), l_is_smaller ? l_size : r_size);
        if (compare_result == 0)
            return l_is_smaller;
        else
            return compare_result < 0;
    }
};

template<typename C, typename T>
struct ska_sort_container_compare<std::basic_string_view<C, T>>
{
    using const_iterator = typename std::basic_string_view<C, T>::const_iterator;
    static bool lexicographical_compare(const_iterator l_it, const_iterator l_end, const_iterator r_it, const_iterator r_end)
    {
        size_t l_size = l_end - l_it;
        size_t r_size = r_end - r_it;
        bool l_is_smaller = l_size < r_size;
        int compare_result = T::compare(std::addressof(*l_it), std::addressof(*r_it), l_is_smaller ? l_size : r_size);
        if (compare_result == 0)
            return l_is_smaller;
        else
            return compare_result < 0;
    }
};

template<typename T>
struct ska_sorter : detail_ska_sort::fallback_ska_sorter<T>
{
};

template<typename It, typename ExtractKey>
void ska_sort(It begin, It end, ExtractKey && extract_key)
{
    detail_ska_sort::ska_sort_with_settings<detail_ska_sort::default_sort_settings>(begin, end, std::forward<ExtractKey>(extract_key));
}
template<typename It>
void ska_sort(It begin, It end)
{
    return ska_sort(begin, end, detail_ska_sort::iterator_identity_function<It>());
}
template<typename It, typename ExtractKey>
void ska_sort_small_key_large_value(It begin, It end, ExtractKey && extract_key)
{
    detail_ska_sort::ska_sort_with_settings_small_key_large_value<detail_ska_sort::default_sort_settings>(begin, end, std::forward<ExtractKey>(extract_key));
}

} // end namespace boost::sort

#endif
