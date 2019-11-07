/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/demultiplex.hpp>

#include <tests/helpers/constrained_types.hpp>
#include <tests/mocks/callback.hpp>
#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

using namespace stream;
using namespace std;

SCENARIO("Sequence writes.")
{
    GIVEN("Two write streams.")
    {
        write_mock writer1, writer2;
        auto       s = demultiplex(writer1, writer2);

        WHEN("A single value is written.")
        {
            REQUIRE_CALL(writer1, write(0)).LR_RETURN(writer1.sender_);
            REQUIRE_CALL(writer2, write(0)).LR_RETURN(writer2.sender_);

            auto sender = s.write(0);
            WHEN("Synchronous submit is called.")
            {
                REQUIRE_CALL(writer1.sender_, submit());
                REQUIRE_CALL(writer2.sender_, submit());

                sender.submit();
            }

            WHEN("Asynchronous submit is called.")
            {
                base_token t1;
                REQUIRE_CALL(writer1.sender_, submit(ANY(base_token)))
                    .LR_SIDE_EFFECT(t1 = _1);

                done_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;

                sender.submit(
                    base_token{error_mock, cancel_mock, callback_mock});

                WHEN("The first callback is invoked.")
                {
                    base_token t2;
                    REQUIRE_CALL(writer2.sender_, submit(ANY(base_token)))
                        .LR_SIDE_EFFECT(t2 = _1);
                    t1.done();

                    WHEN("The second callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call());
                        t2.done();
                    }
                }
            }
        }

        WHEN("A range is written.")
        {
            REQUIRE_CALL(writer1, write_(vector{1, 2}));
            REQUIRE_CALL(writer2, write_(vector{1, 2}));
            array a{1, 2};

            auto sender = s.write(a);
            WHEN("Synchronous submit is called.")
            {
                REQUIRE_CALL(writer1.range_sender_, submit());
                REQUIRE_CALL(writer2.range_sender_, submit());

                sender.submit();
            }

            WHEN("Asynchronous submit is called.")
            {
                base_token t1;
                REQUIRE_CALL(writer1.range_sender_, submit(ANY(base_token)))
                    .LR_SIDE_EFFECT(t1 = _1);

                done_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;

                sender.submit(
                    base_token{error_mock, cancel_mock, callback_mock});

                WHEN("The first callback is invoked.")
                {
                    base_token t2;
                    REQUIRE_CALL(writer2.range_sender_, submit(ANY(base_token)))
                        .LR_SIDE_EFFECT(t2 = _1);
                    t1.done();

                    WHEN("The second callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call());
                        t2.done();
                    }
                }
            }
        }
    }
}

SCENARIO("Cancellation")
{
    GIVEN("Two write streams.")
    {
        write_mock writer1, writer2;
        auto       s = demultiplex(writer1, writer2);

        WHEN("A single value is written.")
        {
            REQUIRE_CALL(writer1, write(0)).LR_RETURN(writer1.sender_);
            REQUIRE_CALL(writer2, write(0)).LR_RETURN(writer2.sender_);

            auto sender = s.write(0);

            WHEN("Asynchronous submit is called.")
            {
                base_token t1;
                REQUIRE_CALL(writer1.sender_, submit(ANY(base_token)))
                    .LR_SIDE_EFFECT(t1 = _1);

                done_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;

                sender.submit(
                    base_token{error_mock, cancel_mock, callback_mock});

                WHEN("The first write is cancelled.")
                {
                    REQUIRE_CALL(writer1.sender_, cancel());
                    sender.cancel();

                    WHEN("The cancel callback is invoked.")
                    {
                        REQUIRE_CALL(cancel_mock, call());
                        t1.cancelled();
                    }
                }

                WHEN("The second write is cancelled.")
                {
                    base_token t2;
                    REQUIRE_CALL(writer2.sender_, submit(ANY(base_token)))
                        .LR_SIDE_EFFECT(t2 = _1);
                    t1.done();

                    REQUIRE_CALL(writer2.sender_, cancel());
                    sender.cancel();

                    WHEN("The cancel callback is invoked.")
                    {
                        REQUIRE_CALL(cancel_mock, call());
                        t2.cancelled();
                    }
                }
            }
        }
    }
}

SCENARIO("Const demultiplex adaptor.")
{
    GIVEN("Two writers.")
    {
        write_mock writer1, writer2;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = demultiplex(writer1, writer2);
        }
    }
}

SCENARIO("R-value writers.")
{
    [[maybe_unused]] auto s =
        demultiplex(move_only_writer{}, move_only_writer{});
}

SCENARIO("Double demultiplex.")
{
    GIVEN("Four write streams.")
    {
        write_mock w[4];

        auto s = demultiplex(demultiplex(w[0], w[1]), demultiplex(w[2], w[3]));

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(w[0], write(0)).LR_RETURN(w[0].sender_);
            REQUIRE_CALL(w[1], write(0)).LR_RETURN(w[1].sender_);
            REQUIRE_CALL(w[2], write(0)).LR_RETURN(w[2].sender_);
            REQUIRE_CALL(w[3], write(0)).LR_RETURN(w[3].sender_);

            auto sender = s.write(0);

            WHEN("Synchronous submit is called.")
            {
                REQUIRE_CALL(w[0].sender_, submit());
                REQUIRE_CALL(w[1].sender_, submit());
                REQUIRE_CALL(w[2].sender_, submit());
                REQUIRE_CALL(w[3].sender_, submit());
                sender.submit();
            }

            WHEN("Asynchronous submit is called.")
            {
                base_token t;
                REQUIRE_CALL(w[0].sender_, submit(ANY(base_token)))
                    .LR_SIDE_EFFECT(t = _1);

                done_callback_mock   done_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(base_token{error_mock, cancel_mock, done_mock});

                REQUIRE_CALL(w[1].sender_, submit(ANY(base_token)))
                    .LR_SIDE_EFFECT(t = _1);
                t.done();
                REQUIRE_CALL(w[2].sender_, submit(ANY(base_token)))
                    .LR_SIDE_EFFECT(t = _1);
                t.done();
                REQUIRE_CALL(w[3].sender_, submit(ANY(base_token)))
                    .LR_SIDE_EFFECT(t = _1);
                t.done();

                REQUIRE_CALL(done_mock, call());
                t.done();
            }
        }
    }
}
