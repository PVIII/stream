/*
 * writestream.h
 *
 *  Created on: 10 Sep 2018
 *      Author: pait
 */

#ifndef TEST_MOCKS_WRITESTREAM_H_
#define TEST_MOCKS_WRITESTREAM_H_

#include <experimental/ranges/algorithm>
#include <experimental/ranges/concepts>
#include <vector>

#include "stream/callback.h"

namespace stream
{
struct write_stream
{
    using value_type = char;

    value_type              v_ = 0x7F;
    std::vector<value_type> vs_;

    void write(value_type v) { v_ = v; }

    void write(std::experimental::ranges::Range const& r)
    {
        vs_.clear();
        std::experimental::ranges::copy(
            r, std::experimental::ranges::back_inserter(vs_));
    }

    void write(value_type v, completion_token c)
    {
        write(v);
        c(0, 1);
    }

    void write(std::experimental::ranges::Range const& r, completion_token c)
    {
        write(r);
        c(0, r.size());
    }
};

} /* namespace stream */

#endif // TEST_MOCKS_WRITESTREAM_H_
