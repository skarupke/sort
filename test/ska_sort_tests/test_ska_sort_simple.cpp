#include <boost/sort/ska_sort/ska_sort_base.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include "test_common.hpp"

#include <vector>
#include <deque>

void test_uint8()
{
    std::vector<uint8_t> to_sort = { 5, 6, 19, 2, 5, 0, 7, 23, 6, 255, 8, 99 };
    test_default_ska_sort_combinations(to_sort);
}
void test_uint8_256_items()
{
    std::vector<uint8_t> to_sort(256, 255);
    to_sort.back() = 254;
    test_default_ska_sort_combinations(to_sort);
}
void test_int8()
{
    std::vector<int8_t> to_sort = { 5, 6, 19, -4, 2, 5, 0, -55, 7, 23, 6, 8, 127, -128, 99 };
    test_default_ska_sort_combinations(to_sort);
}
void test_text()
{
    std::string to_sort = "Hello, World!";
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
void test_int16()
{
    std::vector<int16_t> to_sort = { 5, 6, 19, -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, -32768, 32767, 99 };
    test_default_ska_sort_combinations(to_sort);
}
void test_uint16()
{
    std::vector<uint16_t> to_sort = { 5, 6, 19, 2, 5, 7, 0, 23, 6, 256, 255, 8, 99, 1024, 65535, 65534 };
    test_default_ska_sort_combinations(to_sort);
}
void test_int32()
{
    std::vector<int32_t> to_sort = { 5, 6, 19, -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, std::numeric_limits<int>::lowest(), (std::numeric_limits<int>::max)(), (std::numeric_limits<int>::max)() - 1, std::numeric_limits<int>::lowest() + 1 };
    test_default_ska_sort_combinations(to_sort);
}
void test_uint32()
{
    std::vector<uint32_t> to_sort = { 5, 6, 19, 2, 5, 7, 0, 23, 6, 256, 255, 8, 99, 1024, 65536, 65535, 65534, 1000000, (std::numeric_limits<unsigned int>::max)() };
    test_default_ska_sort_combinations(to_sort);
}
void test_int64()
{
    std::vector<int64_t> to_sort = { 5, 6, 19, std::numeric_limits<std::int32_t>::lowest() + 1, std::numeric_limits<int64_t>::lowest(), -1000000000000, 1000000000000, (std::numeric_limits<int32_t>::max)(), (std::numeric_limits<int64_t>::max)(), -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, std::numeric_limits<int>::lowest(), (std::numeric_limits<int>::max)(), (std::numeric_limits<int>::max)() - 1, std::numeric_limits<int>::lowest() + 1 };
    test_default_ska_sort_combinations(to_sort);
}
void test_uint64()
{
    std::vector<uint64_t> to_sort = { 5, 6, 19, 2, 5, 7, 0, (std::numeric_limits<uint32_t>::max)() + 1, 1000000000000, (std::numeric_limits<uint64_t>::max)(), 23, 6, 256, 255, 8, 99, 1024, 65536, 65535, 65534, 1000000, (std::numeric_limits<unsigned int>::max)() };
    test_default_ska_sort_combinations(to_sort);
    test_small_key_ska_sort_combinations(to_sort);
}

void test_error_case()
{
    std::vector<int8_t> data = { 46, 7, 33, -78, -114, -78, 33, 82 };
    test_default_ska_sort_combinations(data);
}

void test_another_error_case()
{
    std::vector<int8_t> data = { -104, 50, 108, 105, 112, 53, 47, 102 };
    test_default_ska_sort_combinations(data);
}

void test_bytes()
{
    std::vector<unsigned char> data = { 1, 3, 2, 50, 2, 3, 70, 3, 255, 2 };
    test_default_ska_sort_combinations(data);
}
void test_chars()
{
    std::vector<char> data = { 'a', 'z', 'b', 'r', 'w', 'v', 'x', 'u', ',', '4', '2', '7', 'W', '_' };
    test_default_ska_sort_combinations(data);
}
void test_unsigned_short()
{
    std::vector<unsigned short> data = { 1, 0, 65535, 16000, 16001, 16002, 16001, 16005, 16010, 16007, 10, 70, 3 };
    test_default_ska_sort_combinations(data);
}
void test_short()
{
    std::vector<short> data = { 1, 0, (std::numeric_limits<short>::max)(), -10, std::numeric_limits<short>::lowest(), 16000, -16000, 16001, 16002, 16001, 16005, 16010, 16007, 10, 70, 3 };
    test_default_ska_sort_combinations(data);
}
void test_vector_bool()
{
    std::vector<bool> data = { true, false, true, true, false, false, true, true, true, true, false };
    test_default_ska_sort_combinations(data);
}
void test_deque_bool()
{
    std::deque<bool> data = { true, false, true, true, false, false, true, true, true, true, false };
    test_default_ska_sort_combinations(data);
}
void test_reference()
{
    std::vector<int> to_sort = { 6, 5, 4, 3, 2, 1 };
    test_default_ska_sort_combinations(to_sort, [](int & i) -> int & { return i; });
}

void test_pointers()
{
    int array[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int * > to_sort =
    {
        array + 1,
        array + 3,
        array,
        array + 2,
        array + 7,
        array + 8,
        array + 6,
        array + 4,
        array + 5
    };
    test_default_ska_sort_combinations(to_sort);
}



int test_main(int, char **)
{
    test_uint8();
    test_uint8_256_items();
    test_int8();
    test_text();
    test_u16string();
    test_u32string();
    test_wstring();
    test_int16();
    test_uint16();
    test_int32();
    test_uint32();
    test_int64();
    test_uint64();
    test_error_case();
    test_another_error_case();
    test_bytes();
    test_chars();
    test_unsigned_short();
    test_short();
    test_vector_bool();
    test_deque_bool();
    test_reference();
    test_pointers();

    return 0;
}


