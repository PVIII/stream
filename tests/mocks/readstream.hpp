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

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/concepts>
#include <vector>

namespace ranges = std::experimental::ranges;

namespace stream
{
struct read_mock
{
    using value_type = int;

    struct sender
    {
        MAKE_MOCK0(submit, value_type());
        MAKE_MOCK1(submit, void(read_token<value_type>&&));
    };
    struct range_sender
    {
        MAKE_MOCK0(submit, void());
        MAKE_MOCK1(submit, void(completion_token&&));
    };
    sender       sender_;
    range_sender range_sender_;

    MAKE_MOCK0(read, sender&());
    range_sender& read(ranges::Range&& r)
    {
        std::vector<value_type> v;
        read_(v);
        ranges::copy(v, ranges::begin(r));
        return range_sender_;
    }
    MAKE_MOCK1(read_, void(std::vector<value_type>& r));
};

} // namespace stream

#endif // TEST_MOCKS_READSTREAM_HPP_
