/*
 * readstream.h
 *
 *  Created on: Apr 12, 2019
 *      Author: patrick
 */

#ifndef TEST_MOCKS_READSTREAM_H
#define TEST_MOCKS_READSTREAM_H

#include <experimental/ranges/concepts>
#include <vector>

#include "stream/callback.h"

namespace stream
{
struct read_stream
{
    using value_type = char;

    value_type              v_ = 0x7F;
    std::vector<value_type> vs_;

    value_type read() { return v_; }

    void read(auto& r) { std::experimental::ranges::copy(vs_, std::begin(r)); }
};

} /* namespace stream */

#endif // TEST_MOCKS_READSTREAM_H
