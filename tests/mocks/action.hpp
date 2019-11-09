/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_ACTION_HPP_
#define TESTS_MOCKS_ACTION_HPP_

#include <tests/mocks/callback.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

namespace stream
{
struct action_mock
{
    struct sender
    {
        MAKE_MOCK0(submit, void());
        MAKE_MOCK1(submit, void(token<>));
        MAKE_MOCK0(cancel, void());
    };
    sender sender_;

    sender& operator()() const { return call(); }
    MAKE_CONST_MOCK0(call, sender&());
};
} // namespace stream

#endif /* TESTS_MOCKS_ACTION_HPP_ */
