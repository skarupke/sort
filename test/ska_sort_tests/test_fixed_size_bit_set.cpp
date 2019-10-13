#include <boost/sort/ska_sort/detail/fixed_size_bit_set.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <set>

static constexpr const std::uint64_t starting_values[] =
{
    1, 101, 99, 7, std::numeric_limits<std::uint64_t>::max()
};

void test_find_lsb()
{
    for (unsigned i = 0; i < 64; ++i)
    {
        for (std::uint64_t v : starting_values)
        {
            v <<= i;
            BOOST_CHECK(boost::sort::detail_ska_sort::find_lsb(v) == i);
        }
    }
}
void test_find_lsb_float_cast()
{
    // this is the backup algorithm when there is no built-in function.
    // re-implementing it here to test it
    auto bit_scan_forward = [](std::uint64_t v)
    {
        union
        {
            float as_float;
            uint32_t as_uint32;
        }
        in_union = { static_cast<float>(v & -v) }; // cast the least significant bit in v to a float
        return (in_union.as_uint32 >> 23) - 0x7f;
    };
    for (unsigned i = 0; i < 64; ++i)
    {
        for (std::uint64_t v : starting_values)
        {
            v <<= i;
            BOOST_CHECK(bit_scan_forward(v) == i);
        }
    }
}

void test_fixed_size_bit_set()
{
    std::set<int> bits_to_set = { 0, 1, 3, 7, 12, 20, 50, 60, 70, 99, 150, 200, 255 };
    boost::sort::detail_ska_sort::fixed_size_bit_set<256> bit_set;
    for (int to_set : bits_to_set)
        bit_set.set_bit(to_set);
    bit_set.for_each_set_bit([&](int bit)
    {
        BOOST_CHECK(bits_to_set.erase(bit));
    });
    BOOST_CHECK(bits_to_set.empty());
}

int test_main(int, char **)
{
    test_find_lsb();
    test_find_lsb_float_cast();
    test_fixed_size_bit_set();

    return 0;
}


