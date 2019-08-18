/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_CALLBACK_HPP_
#define TESTS_MOCKS_CALLBACK_HPP_

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

namespace stream
{
struct write_callback_mock
{
    void operator()(stream::error_code e) { return call(e); }
    MAKE_MOCK1(call, void(stream::error_code));
};

struct read_callback_mock
{
    void operator()(stream::error_code e, char v) { return call(e, v); }
    MAKE_MOCK2(call, void(stream::error_code, char v));
};

struct range_callback_mock
{
    void operator()(stream::error_code e, std::size_t n) { return call(e, n); }
    MAKE_MOCK2(call, void(stream::error_code, std::size_t));
};

} // namespace stream

#endif /* TESTS_MOCKS_CALLBACK_HPP_ */
