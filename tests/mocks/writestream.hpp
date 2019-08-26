/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_WRITESTREAM_HPP_
#define TESTS_MOCKS_WRITESTREAM_HPP_

#include <libstream/callback.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <array>
#include <cassert>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>
#include <vector>

namespace ranges = std::experimental::ranges;

namespace stream
{
struct write_mock
{
    struct sender
    {
        MAKE_MOCK0(submit, void());
        MAKE_MOCK1(submit, void(write_token&&));
    };
    struct range_sender
    {
        MAKE_MOCK0(submit, void());
        MAKE_MOCK1(submit, void(completion_token&&));
    };
    bool         check_range_type_ = false;
    sender       sender_{};
    range_sender range_sender_{};

    MAKE_MOCK1(write, sender&(int));

    MAKE_MOCK0(contiguous_write_, void());
    MAKE_MOCK0(random_access_write_, void());
    MAKE_MOCK0(bidirectional_write_, void());
    MAKE_MOCK1(write_, void(std::vector<int>));
    template<ranges::InputRange R> range_sender& write(R&& r)
    {
        if(check_range_type_)
        {
            if constexpr(ranges::ContiguousRange<R>) { contiguous_write_(); }
            else if constexpr(ranges::RandomAccessRange<R>)
            {
                random_access_write_();
            }
            else if constexpr(ranges::BidirectionalRange<R>)
            {
                bidirectional_write_();
            }
        }
        write_(std::vector<int>{ranges::begin(r), ranges::end(r)});
        return range_sender_;
    }
};

} // namespace stream

#endif // TESTS_MOCKS_WRITESTREAM_HPP_
