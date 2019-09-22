/*
 * action.hpp
 *
 *  Created on: Sep 22, 2019
 *      Author: patrick
 */

#ifndef TESTS_MOCKS_ACTION_HPP_
#define TESTS_MOCKS_ACTION_HPP_

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

namespace stream
{
struct action_mock
{
    void operator()() const { return call(); }
    MAKE_CONST_MOCK0(call, void());
};
}

#endif /* TESTS_MOCKS_ACTION_HPP_ */
