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

struct write_stream
{
    using value_type = char;

    value_type              v_ = 0x7F;
    std::vector<value_type> vs_;

    write_token      callback_;
    completion_token range_callback_;
    std::size_t      n_;

    template<ranges::Range R> struct range_context
    {
        R             range_;
        write_stream& stream_;
        bool          submitted_ = false;

        void submit(completion_token&& t)
        {
            assert(!submitted_);

            submitted_              = true;
            stream_.range_callback_ = t;

            stream_.vs_.clear();
            ranges::copy(range_, ranges::back_inserter(stream_.vs_));
            stream_.n_ = stream_.vs_.size();
        }

        void submit() { submit({}); }
    };

    struct write_context
    {
        value_type    value_;
        write_stream& stream_;

        void submit(write_token&& t)
        {
            stream_.callback_ = t;
            stream_.v_        = value_;
        }

        void submit() { submit({}); }
    };

    auto write(value_type v) { return write_context{v, *this}; }

    template<ranges::Range R> auto write(R&& r)
    {
        return range_context<R>{std::forward<R>(r), *this};
    }

    void callback() const { callback_(0); }

    void range_callback() const { range_callback_(0, n_); }
};

} // namespace stream

#endif // TESTS_MOCKS_WRITESTREAM_HPP_
