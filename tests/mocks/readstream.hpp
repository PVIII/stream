/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TEST_MOCKS_READSTREAM_H_
#define TEST_MOCKS_READSTREAM_H_

#include <libstream/callback.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <experimental/ranges/algorithm>
#include <experimental/ranges/concepts>
#include <vector>

namespace ranges = std::experimental::ranges;

namespace stream
{
struct read_mock
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
        MAKE_MOCK1(submit, void(base_token&&));
        MAKE_MOCK0(cancel, void());
    };
    sender       sender_;
    range_sender range_sender_;

    MAKE_MOCK0(read, sender&());
    range_sender& read(ranges::Range&& r)
    {
        std::vector<int> v;
        read_(v);
        ranges::copy(v, ranges::begin(r));
        return range_sender_;
    }
    MAKE_MOCK1(read_, void(std::vector<int>& r));
};

} // namespace stream

#endif // TEST_MOCKS_READSTREAM_HPP_
