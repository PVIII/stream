/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_HELPERS_SUBMIT_TESTER_HPP_
#define TESTS_HELPERS_SUBMIT_TESTER_HPP_

#include <tests/mocks/callback.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

using trompeloeil::_;

namespace stream
{

void test_sync_submit(auto& mock_sender, auto& sender)
{
    WHEN("Synchronous submit is called on the sender.")
    {
        ALLOW_CALL(mock_sender, submit());
        sender.submit();
    }
}

void test_sync_read_submit(auto& mock_sender, auto& sender, auto produced,
                           auto expected)
{
    WHEN("Synchronous submit is called on the sender.")
    {
        ALLOW_CALL(mock_sender, submit()).RETURN(produced);
        REQUIRE(sender.submit() == expected);
    }
}

void test_async_write_submit(auto& mock_sender, auto& sender)
{
    WHEN("Asynchronous submit is called on the sender.")
    {
        write_token t;
        ALLOW_CALL(mock_sender, submit(ANY(write_token)))
            .LR_SIDE_EFFECT(t = _1;);
        write_callback_mock callback_mock;
        error_callback_mock error_mock;

        sender.submit(write_token{error_mock, callback_mock});

        AND_WHEN("The callback is invoked.")
        {
            REQUIRE_CALL(callback_mock, call());
            t.done();
        }
    }
}

void test_async_range_submit(auto& mock_sender, auto& sender, std::size_t produced_size, std::size_t expected_size)
{
    WHEN("Asynchronous submit is called.")
    {
        completion_token t;
        ALLOW_CALL(mock_sender, submit(ANY(completion_token)))
            .LR_SIDE_EFFECT(t = _1);
        range_callback_mock callback_mock;
        error_callback_mock error_mock;

        sender.submit(completion_token{error_mock, callback_mock});

        AND_WHEN("The callback is invoked.")
        {
            REQUIRE_CALL(callback_mock, call(expected_size));
            t.done(produced_size);
        }
    }
}

template<typename T>
void test_async_read_submit(auto& mock_sender, auto& sender, auto produced, T expected)
{
    WHEN("Asynchronous submit is called on the sender.")
    {
        read_token<T> t;
        ALLOW_CALL(mock_sender, submit(ANY(read_token<T>)))
            .LR_SIDE_EFFECT(t = _1);
        read_callback_mock callback_mock;
        error_callback_mock error_mock;

        sender.submit(read_token<int>{error_mock, callback_mock});

        AND_WHEN("The callback is invoked.")
        {
            REQUIRE_CALL(callback_mock, call(expected));
            t.done(produced);
        }
    }
}
}

#endif /* TESTS_HELPERS_SUBMIT_TESTER_HPP_ */
