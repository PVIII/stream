/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_WRITESTREAM_HPP_
#define TESTS_MOCKS_WRITESTREAM_HPP_

#include <libstream/callback.hpp>

#include <cassert>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>
#include <vector>

namespace ranges = std::experimental::ranges;

namespace stream
{
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
            stream_.write(range_);
        }
    };

    void write(value_type v) { v_ = v; }

    void write(ranges::Range const& r)
    {
        vs_.clear();
        ranges::copy(r, ranges::back_inserter(vs_));
        n_ = vs_.size();
    }

    void write(value_type v, write_token c)
    {
        write(v);
        callback_ = c;
        n_        = 1;
    }

    template<ranges::Range R> auto write(R&& r, completion_token c)
    {
        range_callback_ = c;
        return range_context<R>{std::forward<R>(r), *this};
    }

    template<ranges::Range R> auto write_async(R&& r)
    {
        return range_context<R>{std::forward<R>(r), *this};
    }

    void callback() const { callback_(0); }

    void range_callback() const { range_callback_(0, n_); }
};

} // namespace stream

#endif // TESTS_MOCKS_WRITESTREAM_HPP_
