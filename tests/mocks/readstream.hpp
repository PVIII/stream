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

    read_token<value_type> callback_;

    value_type read() { return v_; }

    void read(auto& r) { ranges::copy(vs_, std::begin(r)); }

    void read(read_token<value_type> t) { callback_ = t; }

    void callback() const { callback_(0, v_); }
};

} // namespace stream

#endif // TEST_MOCKS_READSTREAM_HPP_
