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

    void write(value_type v) { v_ = v; }

    void write(ranges::Range const& r)
    {
        vs_.clear();
        ranges::copy(r, ranges::back_inserter(vs_));
    }

    void write(value_type v, completion_token c)
    {
        write(v);
        c(0, 1);
    }

    void write(ranges::SizedRange const& r, completion_token c)
    {
        write(r);
        c(0, r.size());
    }
};

} // namespace stream

#endif // TESTS_MOCKS_WRITESTREAM_HPP_
