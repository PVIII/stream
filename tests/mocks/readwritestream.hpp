/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TEST_MOCKS_READWRITESTREAM_H_
#define TEST_MOCKS_READWRITESTREAM_H_

#include <libstream/callback.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <experimental/ranges/algorithm>
#include <experimental/ranges/concepts>
#include <vector>

namespace ranges = std::experimental::ranges;

namespace stream
{
struct read_write_mock
{
    struct sender
    {
        MAKE_MOCK0(submit, int());
        MAKE_MOCK1(submit, void(read_token<int>&&));
        MAKE_MOCK0(cancel, void());
    };
    struct range_sender
    {
        MAKE_MOCK0(submit, void());
        MAKE_MOCK1(submit, void(completion_token&&));
        MAKE_MOCK0(cancel, void());
    };
    sender       sender_;
    range_sender range_sender_;

    MAKE_MOCK1(readwrite, sender&(int));
    
    MAKE_MOCK2(readwrite_, void(std::vector<int>,std::vector<int>&));
    range_sender& readwrite(ranges::InputRange&& rin, ranges::Range&& rout)
    {
        std::vector<int> v;
        readwrite_(std::vector<int>{ranges::begin(rin), ranges::end(rin)}, v);
        ranges::copy(v, ranges::begin(rout));
        return range_sender_;
    }
};

} // namespace stream

#endif // TEST_MOCKS_READWRITESTREAM_HPP_
