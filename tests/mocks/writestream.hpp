/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_WRITESTREAM_HPP_
#define TESTS_MOCKS_WRITESTREAM_HPP_

#include <libstream/callback.hpp>

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

    void write(value_type v) { v_ = v; }

    void write(ranges::Range const& r)
    {
        vs_.clear();
        ranges::copy(r, ranges::back_inserter(vs_));
    }

    void write(value_type v, write_token c)
    {
        write(v);
        callback_ = c;
        n_        = 1;
    }

    void write(ranges::SizedRange const& r, completion_token c)
    {
        write(r);
        range_callback_ = c;
        n_              = r.size();
    }

    void callback() const { callback_(0); }

    void range_callback() const { range_callback_(0, n_); }
};

} // namespace stream

#endif // TESTS_MOCKS_WRITESTREAM_HPP_
