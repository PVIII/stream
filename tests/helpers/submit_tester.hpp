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
constexpr error_code dummy_error = 1;

template<class P, class E> struct test_pair
{
    P produced_;
    E expected_;

    test_pair(P p, E e) : produced_(p), expected_(e) {}
    template<class P2, class E2>
    test_pair(test_pair<P2, E2> p)
        : produced_(p.produced_), expected_(p.expected_)
    {
    }
};

template<class P, class E> test_pair(P, E)->test_pair<P, E>;

void test_sync_submit(auto& mock_sender, auto& sender)
{
    WHEN("Synchronous submit is called on the sender.")
    {
        REQUIRE_CALL(mock_sender, submit());
        sender.submit();
    }
}

template<class P, class E>
void test_sync_read_submit(auto& mock_sender, auto& sender, test_pair<P, E> p)
{
    WHEN("Synchronous submit is called on the sender.")
    {
        REQUIRE_CALL(mock_sender, submit()).RETURN(p.produced_);
        REQUIRE(sender.submit() == p.expected_);
    }
}

void test_async_write_submit(auto& mock_sender, auto& sender,
                             stream::error_code e = 0)
{
    WHEN("Asynchronous submit is called on the sender.")
    {
        base_token t;
        REQUIRE_CALL(mock_sender, submit(ANY(base_token)))
            .LR_SIDE_EFFECT(t = _1;);
        done_callback_mock   callback_mock;
        error_callback_mock  error_mock;
        cancel_callback_mock cancel_mock;

        sender.submit(base_token{error_mock, cancel_mock, callback_mock});

        WHEN("The callback is invoked.")
        {
            if(e == 0)
            {
                REQUIRE_CALL(callback_mock, call());
                t.done();
            }
            else
            {
                REQUIRE_CALL(error_mock, call(e));
                t.error(e);
            }
        }
    }
}

void test_async_range_submit(auto& mock_sender, auto& sender,
                             stream::error_code e = 0)
{
    WHEN("Asynchronous submit is called.")
    {
        base_token t;
        REQUIRE_CALL(mock_sender, submit(ANY(base_token)))
            .LR_SIDE_EFFECT(t = _1);
        done_callback_mock   callback_mock;
        error_callback_mock  error_mock;
        cancel_callback_mock cancel_mock;

        sender.submit(base_token{error_mock, cancel_mock, callback_mock});

        AND_WHEN("The callback is invoked.")
        {
            if(e == 0)
            {
                REQUIRE_CALL(callback_mock, call());
                t.done();
            }
            else
            {
                REQUIRE_CALL(error_mock, call(e));
                t.error(e);
            }
        }
    }
}

template<class P, class E>
void test_async_read_submit(auto& mock_sender, auto& sender, test_pair<P, E> p,
                            stream::error_code e = 0)
{
    WHEN("Asynchronous submit is called on the sender.")
    {
        read_token<E> t;
        REQUIRE_CALL(mock_sender, submit(ANY(read_token<E>)))
            .LR_SIDE_EFFECT(t = _1);
        read_callback_mock   callback_mock;
        error_callback_mock  error_mock;
        cancel_callback_mock cancel_mock;

        sender.submit(read_token<E>{error_mock, cancel_mock, callback_mock});

        AND_WHEN("The callback is invoked.")
        {
            if(e == 0)
            {
                REQUIRE_CALL(callback_mock, call(p.expected_));
                t.done(p.produced_);
            }
            else
            {
                REQUIRE_CALL(error_mock, call(e));
                t.error(e);
            }
        }
    }
}
} // namespace stream

#endif /* TESTS_HELPERS_SUBMIT_TESTER_HPP_ */
