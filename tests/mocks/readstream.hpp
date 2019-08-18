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
    struct sender
    {
        MAKE_MOCK0(submit, char());
        MAKE_MOCK1(submit, void(read_token<char>&&));
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
        read_(r);
        return range_sender_;
    }
    MAKE_MOCK1(read_, void(std::array<int, 2>& r));
};

struct read_stream
{
    char              v_ = 0x7F;
    std::vector<char> vs_;

    completion_token range_callback_;
    template<ranges::Range R> struct range_context
    {
        R            range_;
        read_stream& stream_;
        bool         submitted_ = false;

        void submit(completion_token&& t)
        {
            assert(!submitted_);

            submitted_              = true;
            stream_.range_callback_ = t;
            ranges::copy(stream_.vs_, std::begin(range_));
        }

        auto submit()
        {
            assert(!submitted_);

            submitted_ = true;
            ranges::copy(stream_.vs_, std::begin(range_));

            return stream_.vs_.size();
        }
    };

    struct context
    {
        read_stream&     stream_;
        read_token<char> token_;

        context(read_stream& s) : stream_(s) {}

        void submit(read_token<char>&& t)
        {
            token_                = t;
            stream_.read_context_ = this;
        }

        auto submit() { return stream_.v_; }

        void callback(char v) const { token_(0, v); }
    };
    context* read_context_ = nullptr;

    auto read() { return context{*this}; }

    template<ranges::Range R> auto read(R&& r)
    {
        return range_context<R>{r, *this};
    }

    void range_callback() const { range_callback_(0, vs_.size()); }

    void read_callback(char v) const { read_context_->callback(v); }
};

} // namespace stream

#endif // TEST_MOCKS_READSTREAM_HPP_
