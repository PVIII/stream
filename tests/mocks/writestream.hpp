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
    sender       sender_;
    range_sender range_sender_;

    MAKE_MOCK1(write, sender&(int));
    range_sender& write(ranges::Range&& r)
    {
        std::vector<int> v;
        ranges::copy(r, ranges::back_inserter(v));
        write_(v);
        return range_sender_;
    }
    MAKE_MOCK1(write_, void(std::vector<int>));
};

} // namespace stream

#endif // TESTS_MOCKS_WRITESTREAM_HPP_
