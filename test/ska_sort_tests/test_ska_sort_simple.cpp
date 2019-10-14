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
    std::vector<int32_t> to_sort = { 5, 6, 19, -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() - 1, std::numeric_limits<int>::lowest() + 1 };
    test_default_ska_sort_combinations(to_sort);
}
void test_uint32()
{
    std::vector<uint32_t> to_sort = { 5, 6, 19, 2, 5, 7, 0, 23, 6, 256, 255, 8, 99, 1024, 65536, 65535, 65534, 1000000, std::numeric_limits<unsigned int>::max() };
    test_default_ska_sort_combinations(to_sort);
}
void test_int64()
{
    std::vector<int64_t> to_sort = { 5, 6, 19, std::numeric_limits<std::int32_t>::lowest() + 1, std::numeric_limits<ino64_t>::lowest(), -1000000000000, 1000000000000, std::numeric_limits<int32_t>::max(), std::numeric_limits<int64_t>::max(), -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() - 1, std::numeric_limits<int>::lowest() + 1 };
    test_default_ska_sort_combinations(to_sort);
}
void test_uint64()
{
    std::vector<uint64_t> to_sort = { 5, 6, 19, 2, 5, 7, 0, std::numeric_limits<uint32_t>::max() + 1, 1000000000000, std::numeric_limits<uint64_t>::max(), 23, 6, 256, 255, 8, 99, 1024, 65536, 65535, 65534, 1000000, std::numeric_limits<unsigned int>::max() };
    test_default_ska_sort_combinations(to_sort);
    test_small_key_ska_sort_combinations(to_sort);
}
void test_float()
{
    std::vector<float> to_sort = { 5, 6, 19, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, 0.1f, 2.5f, 17.8f, -12.4f, -0.0000002f, -0.0f, -777777777.7f, 444444444444.4f };
    test_default_ska_sort_combinations(to_sort);
    test_small_key_ska_sort_combinations(to_sort);
}
void test_double()
{
    std::vector<double> to_sort = { 5, 6, 19, std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, 0.1, 2.5, 17.8, -12.4, -0.0000002, -0.0, -777777777.7, 444444444444.4 };
    test_default_ska_sort_combinations(to_sort);
}
void test_float_backwards()
{
    std::vector<float> to_sort = { 5, 6, 19, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, 0.1f, 2.5f, 17.8f, -12.4f, -0.0000002f, -0.0f, -777777777.7f, 444444444444.4f };
    test_default_ska_sort_combinations(to_sort, [](float f){ return -f; });
    test_small_key_ska_sort_combinations(to_sort, [](float f){ return -f; });
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
    std::vector<short> data = { 1, 0, std::numeric_limits<short>::max(), -10, std::numeric_limits<short>::lowest(), 16000, -16000, 16001, 16002, 16001, 16005, 16010, 16007, 10, 70, 3 };
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
void test_reference()
{
    std::vector<int> to_sort = { 6, 5, 4, 3, 2, 1 };
    test_default_ska_sort_combinations(to_sort, [](int & i) -> int & { return i; });
}
void test_pair_reference()
{
    std::vector<std::pair<int, bool>> to_sort = { { 5, true }, { 5, false }, { 6, false }, { 7, true }, { 4, false }, { 4, true } };
    test_default_ska_sort_combinations(to_sort, [](auto & i) -> decltype(auto) { return i; });
}

static const std::vector<float> float_bug_input =
{
    -3305.68f, -2684.34f, -2234.62f, -1988.51f, -1965.32f, -1808.13f, -1800.42f, -1722.81f, -1704.51f, -1649.78f,
    -1591.76f, -1581.61f, -1566.55f, -1459.65f, -1382.87f, -1333.64f, -1319.45f, -1186.65f, -1098.14f, -1039.01f,
    -973.805f, -951.14f, -926.392f, -920.247f, -922.534f, -922.763f, -919.381f, -850.919f, -796.569f, -787.428f, -780.044f,
    -772.276f, -756.935f, -720.871f, -673.421f, -664.041f, -628.639f, -580.13f, -519.876f, -508.197f, -507.302f, -471.265f,
    -464.019f, -429.188f, -410.241f, -366.956f, -365.482f, -327.256f, -299.654f, -295.048f, -278.138f, -262.674f, -205.262f,
    -192.724f, -164.104f, -145.508f, -140.691f, -133.544f, -44.4212f, -20.0017f, -0.324011f, 19.3389f, 33.6625f, 79.592f,
    98.4721f, 111.029f, 142.54f, 158.674f, 166.794f, 183.275f, 187.558f, 206.012f, 217.915f, 225.862f, 240.096f, 248.95f,
    287.886f, 287.903f, 303.676f, 329.983f, 355.284f, 355.761f, 401.808f, 422.257f, 440.037f, 447.795f, 485.2f, 513.239f,
    530.438f, 540.15f, 551.101f, 591.771f, 604.096f, 627.521f, 643.663f, 644.669f, 729.3f, 740.218f, 747.142f, 754.765f,
    794.868f, 871.391f, 918.005f, 921.496f, 924.987f, 937.024f, 937.706f, 992.288f, 995.07f, 1000.17f, 1007.06f, 1062.04f,
    1069.21f, 1072.04f, 1162.66f, 1173.71f, 1180.34f, 1184.88f, 1249.75f, 1263.56f, 1298.19f, 1312.38f, 1365.59f, 1368.41f,
    1431.62f, 1462.83f, 1470.19f, 1508.33f
};
void test_float_bug()
{
    std::vector<float> to_sort(float_bug_input);
    test_default_ska_sort_combinations(to_sort);
    std::vector<std::pair<bool, float>> in_pair;
    for (size_t i = 0; i < float_bug_input.size(); ++i)
    {
        in_pair.emplace_back(i < float_bug_input.size() / 2, float_bug_input[i]);
    }
    test_default_ska_sort_combinations(in_pair);
}
void test_float_negative_zero()
{
    std::vector<std::pair<float, int>> to_sort =
    {
        { 0.0f, 1.0f },
        { -0.0f, 2.0f }
    };
    test_default_ska_sort_combinations(to_sort);
}
void test_float_negative_zero_after_first_byte()
{
    std::vector<float> to_sort =
    {
        -0.0f, 0.0f, -0.0f, std::numeric_limits<float>::min(),
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    };
    test_default_ska_sort_combinations(to_sort);
}
void test_float_negative_zero_after_second_byte()
{
    union
    {
        uint32_t as_uint;
        float as_float;
    }
    last_byte = { 0x00000001 };
    union
    {
        uint32_t as_uint;
        float as_float;
    }
    second_to_last_byte = { 0x00000100 };
    std::vector<float> to_sort =
    {
        -0.0f, 0.0f, -0.0f, std::numeric_limits<float>::min(), last_byte.as_float,
        last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float,
        second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float,
        last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float, last_byte.as_float,
        second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float, second_to_last_byte.as_float,
    };
    // todo: I think this should fail right now. change the test until it fails
    test_default_ska_sort_combinations(to_sort);
}

void test_double_first_two_bytes()
{
    std::vector<uint64_t> as_bytes =
    {
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
        0x0000'1000'0000'0000, 0x0000'1000'0000'0000,
        0x0000'0001'0000'0000, 0x0000'0001'0000'0000,
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
        0x0000'0000'0000'0000, 0x8000'0000'0000'0000, 0x0000'0000'0000'0000, 0x8000'0000'0000'0000,
    };
    std::vector<double> to_sort;
    while (!as_bytes.empty())
    {
        to_sort.resize(as_bytes.size());
        memcpy(to_sort.data(), as_bytes.data(), sizeof(double) * to_sort.size());
        test_default_ska_sort_combinations(to_sort);
        as_bytes.pop_back();
    }
}

void test_long_double()
{
    std::vector<long double> to_sort = { 5, 6, 19, std::numeric_limits<long double>::infinity(), -std::numeric_limits<long double>::infinity(), -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, 32768, -32769, -32768, 32767, 99, 1000000, -1000001, 0.1l, 2.5l, 17.8l, -12.4l, -0.0000002l, -0.0l, -777777777.7l, 444444444444.4l, static_cast<long double>(1.0) / static_cast<long double>(3.0), -static_cast<long double>(1.0) / static_cast<long double>(3.0) };
    test_default_ska_sort_combinations(to_sort);
}

void test_long_double_error()
{
    std::vector<long double> to_sort =
    {
        0.0l, 7.42772e-4942l,
        0.0l, 0.0l, 0.0l, 0.0l, 0.0l,
        8.09234e-4949l, 0.0l,
        -4.77987e+986l,
        -4.77987e+986l,
        -5.53471e+1063l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77941e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -1.02204e+937l,
        1.09826e-4934l,
        0.0l, 0.0l, 0.0l, 0.0l, 0.0l,
        -4.7612e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -2.05405e+937l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.77987e+986l,
        -4.76482e+986l,
    };
    test_default_ska_sort_combinations(to_sort);
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
    test_uint8();
    test_uint8_256_items();
    test_int8();
    test_text();
    test_u16string();
    test_u32string();
    test_int16();
    test_uint16();
    test_int32();
    test_uint32();
    test_int64();
    test_uint64();
    test_float();
    test_double();
    test_error_case();
    test_another_error_case();
    test_bytes();
    test_chars();
    test_unsigned_short();
    test_short();
    test_vector_bool();
    test_deque_bool();
    test_pair();
    test_pair_bool();
    test_pair_other_direction();
    test_reference();
    test_pair_reference();
    test_float_bug();
    test_float_negative_zero();
    test_float_negative_zero_after_first_byte();
    test_double_first_two_bytes();
    test_long_double();
    test_long_double_error();
    test_struct();
    test_struct_mem_ptr();
    test_pointers();
    test_small_key();

    return 0;
}


