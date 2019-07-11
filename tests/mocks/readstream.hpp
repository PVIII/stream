/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TEST_MOCKS_READSTREAM_H_
#define TEST_MOCKS_READSTREAM_H_

#include <libstream/callback.hpp>

#include <experimental/ranges/algorithm>
#include <experimental/ranges/concepts>
#include <vector>

namespace ranges = std::experimental::ranges;

namespace stream
{
struct read_stream
{
    using value_type = char;

    value_type              v_ = 0x7F;
    std::vector<value_type> vs_;

    read_token<value_type> read_callback_;

    completion_token range_callback_;
    template<ranges::Range R> struct range_read_context
    {
        R            range_;
        read_stream& stream_;
        bool         submitted_ = false;

        void submit()
        {
            assert(!submitted_);

            submitted_ = true;
            ranges::copy(stream_.vs_, std::begin(range_));
        }
    };

    value_type read() { return v_; }

    std::size_t read(auto& r)
    {
        ranges::copy(vs_, std::begin(r));
        return vs_.size();
    }

    void read(read_token<value_type> t) { read_callback_ = t; }

    template<ranges::Range R> auto read(R&& r, completion_token&& t)
    {
        range_callback_ = t;
        return range_read_context<R>{r, *this};
    }

    void read_callback() const { read_callback_(0, v_); }

    void range_callback() const { range_callback_(0, vs_.size()); }
};

} // namespace stream

#endif // TEST_MOCKS_READSTREAM_HPP_
