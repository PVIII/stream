/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_CALLBACK_HPP_
#define TESTS_MOCKS_CALLBACK_HPP_

#include <libstream/callback.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

namespace stream
{
struct done_callback_mock
{
    void operator()() { return call(); }
    MAKE_MOCK0(call, void());
};

struct read_callback_mock
{
    void operator()(int v) { return call(v); }
    MAKE_MOCK1(call, void(int v));
};

struct error_callback_mock
{
    void operator()(stream::error_code e) { return call(e); }
    MAKE_MOCK1(call, void(stream::error_code));
};

struct cancel_callback_mock
{
    void operator()() { return call(); }
    MAKE_MOCK0(call, void());
};

} // namespace stream

#endif /* TESTS_MOCKS_CALLBACK_HPP_ */
