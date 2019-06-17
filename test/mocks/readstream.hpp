/*
 * readstream.h
 *
 *  Created on: Apr 12, 2019
 *      Author: patrick
 */

#ifndef TEST_MOCKS_READSTREAM_H_
#define TEST_MOCKS_READSTREAM_H_

#include <stream/callback.hpp>

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

    value_type read() { return v_; }

    void read(auto& r) { ranges::copy(vs_, std::begin(r)); }
};

} // namespace stream

#endif // TEST_MOCKS_READSTREAM_HPP_
