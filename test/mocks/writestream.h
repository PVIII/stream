/*
 * writestream.h
 *
 *  Created on: 10 Sep 2018
 *      Author: pait
 */

#ifndef TEST_MOCKS_WRITESTREAM_H_
#define TEST_MOCKS_WRITESTREAM_H_

#include <experimental/ranges/concepts>
#include <iostream>
#include <vector>

#include "delegate/Delegate.h"

namespace stream
{
struct write_stream
{
    using value_type = char;

    char              v_;
    std::vector<char> vs_;

    void write(char v)
    {
        v_ = v;
        std::cout << v << std::endl;
    }

    void write(std::experimental::ranges::Range&& r)
    {
        vs_.clear();
        std::cout << "( ";
        for(auto v : r)
        {
            std::cout << v << " ";
            vs_.push_back(v);
        }
        std::cout << ")" << std::endl;
    }

    void write(char v, SA::delegate<void()> callback)
    {
        write(v);
        callback();
    }

    void write(std::experimental::ranges::Range&& r,
               SA::delegate<void()>               callback)
    {
        write(r);
        callback();
    }
};

} /* namespace stream */

#endif /* TEST_MOCKS_WRITESTREAM_H_ */