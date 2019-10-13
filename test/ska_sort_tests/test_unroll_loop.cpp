#include <boost/sort/ska_sort/detail/algorithm.hpp>

#include <numeric>
#include <boost/test/test_tools.hpp>
#include <boost/test/included/test_exec_monitor.hpp>


int test_main(int, char **)
{
    std::vector<int> to_iterate(20);
    std::iota(to_iterate.begin(), to_iterate.end(), 1);
    int expected = std::accumulate(to_iterate.begin(), to_iterate.end(), 0);
    int loop_count = 0;
    auto loop_body = [&](auto to_add)
    {
        loop_count += *to_add;
    };
    boost::sort::detail_ska_sort::unroll_loop_nonempty<1>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);
    loop_count = 0;
    boost::sort::detail_ska_sort::unroll_loop_nonempty<2>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);
    loop_count = 0;
    boost::sort::detail_ska_sort::unroll_loop_nonempty<3>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);
    loop_count = 0;
    boost::sort::detail_ska_sort::unroll_loop_nonempty<4>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);
    loop_count = 0;
    boost::sort::detail_ska_sort::unroll_loop_nonempty<5>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);
    loop_count = 0;
    boost::sort::detail_ska_sort::unroll_loop_nonempty<6>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);
    loop_count = 0;
    boost::sort::detail_ska_sort::unroll_loop_nonempty<7>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);
    loop_count = 0;
    boost::sort::detail_ska_sort::unroll_loop_nonempty<8>(to_iterate.begin(), to_iterate.size(), loop_body);
    BOOST_CHECK(loop_count == expected);

    return 0;
}


