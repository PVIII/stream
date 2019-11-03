/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/action.hpp>
#include <libstream/transform.hpp>

#include <tests/helpers/range_matcher.hpp>
#include <tests/helpers/submit_tester.hpp>
#include <tests/mocks/action.hpp>
#include <tests/mocks/callback.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/writestream.hpp>

using namespace stream;
using namespace std;
namespace ranges = std::experimental::ranges;
using trompeloeil::_;

SCENARIO("Actions and transformations.")
{
    action_mock closure;

    GIVEN("A write stream.")
    {
        write_mock writer;

        auto s  = stream::transform_write(writer, [](auto v) { return v + 1; });
        auto s2 = action(s, closure);

        WHEN("Single value write is called")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s2.write(1);

            WHEN("Nothing is submitted.") {}
            WHEN("Synchronous submit.")
            {
                REQUIRE_CALL(closure.sender_, submit());
                test_sync_submit(writer.sender_, sender);
            }
            WHEN("Asynchronous submit.")
            {
                token<> closure_token;
                REQUIRE_CALL(closure.sender_, submit(ANY(token<>)))
                    .LR_SIDE_EFFECT(closure_token = _1;);

                done_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    base_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    base_token writer_token;
                    REQUIRE_CALL(writer.sender_, submit(ANY(base_token)))
                        .LR_SIDE_EFFECT(writer_token = _1;);
                    closure_token.done();

                    WHEN("The writer callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call());
                        writer_token.done();
                    }

                    WHEN("The writer error callback is invoked.")
                    {
                        REQUIRE_CALL(error_mock, call(dummy_error));
                        writer_token.error(dummy_error);
                    }
                }
                WHEN("The action error callback is invoked.")
                {
                    REQUIRE_CALL(error_mock, call(dummy_error));
                    closure_token.error(dummy_error);
                }
            }
        }

        WHEN("Range write is called")
        {
            REQUIRE_CALL(writer, write_(vector{2, 3}));
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);

            array a{1, 2};
            auto  sender = s2.write(a);

            WHEN("Nothing is submitted.") {}
            WHEN("Synchronous submit.")
            {
                REQUIRE_CALL(closure.sender_, submit());
                test_sync_submit(writer.range_sender_, sender);
            }
            WHEN("Asynchronous submit.")
            {
                token<> closure_token;
                REQUIRE_CALL(closure.sender_, submit(ANY(token<>)))
                    .LR_SIDE_EFFECT(closure_token = _1;);

                done_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    base_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    base_token writer_token;
                    REQUIRE_CALL(writer.range_sender_, submit(ANY(base_token)))
                        .LR_SIDE_EFFECT(writer_token = _1;);
                    closure_token.done();

                    WHEN("The writer callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call());
                        writer_token.done();
                    }

                    WHEN("The writer error callback is invoked.")
                    {
                        REQUIRE_CALL(error_mock, call(dummy_error));
                        writer_token.error(dummy_error);
                    }
                }
                WHEN("The action error callback is invoked.")
                {
                    REQUIRE_CALL(error_mock, call(dummy_error));
                    closure_token.error(dummy_error);
                }
            }
        }
    }

    GIVEN("A read stream that adds one.")
    {
        read_mock reader;

        auto s  = stream::transform_read(reader, [](auto v) { return v + 1; });
        auto s2 = action(s, closure);

        WHEN("Single read is called")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s2.read();

            WHEN("Nothing is submitted.") {}
            WHEN("Synchronous submit.")
            {
                REQUIRE_CALL(closure.sender_, submit());
                test_sync_read_submit(reader.sender_, sender, test_pair{1, 2});
            }
            WHEN("Asynchronous submit.")
            {
                token<> closure_token;
                REQUIRE_CALL(closure.sender_, submit(ANY(token<>)))
                    .LR_SIDE_EFFECT(closure_token = _1;);

                read_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    read_token<int>{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    read_token<int> reader_token;
                    REQUIRE_CALL(reader.sender_, submit(ANY(read_token<int>)))
                        .LR_SIDE_EFFECT(reader_token = _1;);
                    closure_token.done();

                    WHEN("The reader callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call(2));
                        reader_token.done(1);
                    }

                    WHEN("The reader error callback is invoked.")
                    {
                        REQUIRE_CALL(error_mock, call(dummy_error));
                        reader_token.error(dummy_error);
                    }
                }
                WHEN("The action error callback is invoked.")
                {
                    REQUIRE_CALL(error_mock, call(dummy_error));
                    closure_token.error(dummy_error);
                }
            }
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{1, 2});
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);

            array<int, 2> a;
            auto          sender = s2.read(a);
            REQUIRE_THAT(a, Equals(array{2, 3}));

            WHEN("Nothing is submitted.") {}
            WHEN("Synchronous submit.")
            {
                REQUIRE_CALL(closure.sender_, submit());
                test_sync_submit(reader.range_sender_, sender);
            }
            WHEN("Asynchronous submit.")
            {
                token<> closure_token;
                REQUIRE_CALL(closure.sender_, submit(ANY(token<>)))
                    .LR_SIDE_EFFECT(closure_token = _1;);

                done_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    base_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    base_token reader_token;
                    REQUIRE_CALL(reader.range_sender_, submit(ANY(base_token)))
                        .LR_SIDE_EFFECT(reader_token = _1;);
                    closure_token.done();

                    WHEN("The reader callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call());
                        reader_token.done();
                    }

                    WHEN("The writer error callback is invoked.")
                    {
                        REQUIRE_CALL(error_mock, call(dummy_error));
                        reader_token.error(dummy_error);
                    }
                }
                WHEN("The action error callback is invoked.")
                {
                    REQUIRE_CALL(error_mock, call(dummy_error));
                    closure_token.error(dummy_error);
                }
            }
        }
    }
}

SCENARIO("Cancelling operations.")
{
    GIVEN("A write stream.")
    {
        write_mock  writer;
        action_mock closure;
        auto s  = stream::transform_write(writer, [](auto v) { return v + 1; });
        auto s2 = action(s, closure);

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);

            auto sender = s2.write(1);

            WHEN("Asynchronous submit.")
            {
                token<> closure_token;
                REQUIRE_CALL(closure.sender_, submit(ANY(token<>)))
                    .LR_SIDE_EFFECT(closure_token = _1;);

                done_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    base_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked")
                {
                    base_token writer_token;
                    REQUIRE_CALL(writer.sender_, submit(ANY(base_token)))
                        .LR_SIDE_EFFECT(writer_token = _1;);
                    closure_token.done();

                    WHEN("The writer is cancelled.")
                    {
                        REQUIRE_CALL(writer.sender_, cancel());
                        sender.cancel();

                        WHEN("The callback is not invoked.") {}
                        WHEN("The cancel callback is invoked.")
                        {
                            REQUIRE_CALL(cancel_mock, call());
                            writer_token.cancelled();
                        }
                    }
                }
                WHEN("The action is cancelled.")
                {
                    REQUIRE_CALL(closure.sender_, cancel());
                    sender.cancel();

                    WHEN("The callback is not invoked.") {}
                    WHEN("The cancel callback is invoked.")
                    {
                        REQUIRE_CALL(cancel_mock, call());
                        closure_token.cancelled();
                    }
                }
            }
        }
    }
}

SCENARIO("Pipe interoperability.")
{
    action_mock closure;
    write_mock  writer;

    auto s = writer | stream::transform_write([](auto v) { return v + 1; }) |
             action(closure);

    WHEN("Single write is called")
    {
        REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
        REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
        REQUIRE_CALL(closure.sender_, submit());

        auto sender = s.write(1);

        test_sync_submit(writer.sender_, sender);
    }
}
