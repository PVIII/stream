/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include "action.hpp"

#include <tests/helpers/constrained_types.hpp>
#include <tests/helpers/range_matcher.hpp>
#include <tests/helpers/submit_tester.hpp>
#include <tests/mocks/action.hpp>
#include <tests/mocks/callback.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/readwritestream.hpp>
#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>
#include <list>

using namespace stream;
using namespace std;
namespace ranges = std::experimental::ranges;
using trompeloeil::_;

SCENARIO("Simple actions.")
{
    GIVEN("A write stream.")
    {
        write_mock  writer;
        action_mock closure;
        auto        s = action(writer, closure);

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s.write(2);

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

                write_callback_mock  callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    write_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    write_token writer_token;
                    REQUIRE_CALL(writer.sender_, submit(ANY(write_token)))
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

        WHEN("Range write is called.")
        {
            REQUIRE_CALL(writer, write_(vector{2, 3}));
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s.write(array{2, 3});

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

                range_callback_mock  callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    completion_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    completion_token writer_token;
                    REQUIRE_CALL(writer.range_sender_,
                                 submit(ANY(completion_token)))
                        .LR_SIDE_EFFECT(writer_token = _1;);
                    closure_token.done();

                    WHEN("The writer callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call(2));
                        writer_token.done(2);
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

        WHEN("[0, 1, 2] is generated and written.")
        {
            REQUIRE_CALL(writer, write_(vector{0, 1, 2}));
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s.write(ranges::view::iota(0, 3));

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

                range_callback_mock  callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    completion_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    completion_token writer_token;
                    REQUIRE_CALL(writer.range_sender_,
                                 submit(ANY(completion_token)))
                        .LR_SIDE_EFFECT(writer_token = _1;);
                    closure_token.done();

                    WHEN("The writer callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call(2));
                        writer_token.done(2);
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

    GIVEN("A read stream.")
    {
        read_mock   reader;
        action_mock closure;
        auto        s = action(reader, closure);

        WHEN("A single value is read.")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s.read();

            WHEN("Nothing is submitted.") {}
            WHEN("Synchronous submit.")
            {
                REQUIRE_CALL(closure.sender_, submit());
                test_sync_read_submit(reader.sender_, sender, test_pair{1, 1});
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
                        REQUIRE_CALL(callback_mock, call(1));
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
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{2, 3});
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            array a{0, 0};
            auto  sender = s.read(a);
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

                range_callback_mock  callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    completion_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    completion_token reader_token;
                    REQUIRE_CALL(reader.range_sender_,
                                 submit(ANY(completion_token)))
                        .LR_SIDE_EFFECT(reader_token = _1;);
                    closure_token.done();

                    WHEN("The reader callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call(2));
                        reader_token.done(2);
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

    GIVEN("A read-write stream.")
    {
        read_write_mock readwriter;
        action_mock     closure;
        auto            s = action(readwriter, closure);

        WHEN("A single value is read and written.")
        {
            REQUIRE_CALL(readwriter, readwrite(1))
                .LR_RETURN(readwriter.sender_);
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s.readwrite(1);

            WHEN("Nothing is submitted.") {}
            WHEN("Synchronous submit.")
            {
                REQUIRE_CALL(closure.sender_, submit());
                test_sync_read_submit(readwriter.sender_, sender,
                                      test_pair{1, 1});
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
                    read_token<int> readwriter_token;
                    REQUIRE_CALL(readwriter.sender_,
                                 submit(ANY(read_token<int>)))
                        .LR_SIDE_EFFECT(readwriter_token = _1;);
                    closure_token.done();

                    WHEN("The writer callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call(1));
                        readwriter_token.done(1);
                    }

                    WHEN("The writer error callback is invoked.")
                    {
                        REQUIRE_CALL(error_mock, call(dummy_error));
                        readwriter_token.error(dummy_error);
                    }
                }
                WHEN("The action error callback is invoked.")
                {
                    REQUIRE_CALL(error_mock, call(dummy_error));
                    closure_token.error(dummy_error);
                }
            }
        }

        WHEN("Ranges are read and written.")
        {
            REQUIRE_CALL(readwriter, readwrite_(vector{2, 3}, _))
                .SIDE_EFFECT(_2 = vector{4, 5});
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            array a{0, 0};
            auto  sender = s.readwrite(array{2, 3}, a);
            REQUIRE_THAT(a, Equals(array{4, 5}));

            WHEN("Nothing is submitted.") {}
            WHEN("Synchronous submit.")
            {
                REQUIRE_CALL(closure.sender_, submit());
                test_sync_submit(readwriter.range_sender_, sender);
            }
            WHEN("Asynchronous submit.")
            {
                token<> closure_token;
                REQUIRE_CALL(closure.sender_, submit(ANY(token<>)))
                    .LR_SIDE_EFFECT(closure_token = _1;);

                range_callback_mock  callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    completion_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked.")
                {
                    completion_token readwriter_token;
                    REQUIRE_CALL(readwriter.range_sender_,
                                 submit(ANY(completion_token)))
                        .LR_SIDE_EFFECT(readwriter_token = _1;);
                    closure_token.done();

                    WHEN("The callback is invoked.")
                    {
                        REQUIRE_CALL(callback_mock, call(2));
                        readwriter_token.done(2);
                    }

                    WHEN("The error callback is invoked.")
                    {
                        REQUIRE_CALL(error_mock, call(dummy_error));
                        readwriter_token.error(dummy_error);
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
        auto        s = action(writer, closure);

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(writer, write(1)).LR_RETURN(writer.sender_);
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);
            auto sender = s.write(1);

            WHEN("Asynchronous submit.")
            {
                token<> closure_token;
                REQUIRE_CALL(closure.sender_, submit(ANY(token<>)))
                    .LR_SIDE_EFFECT(closure_token = _1;);

                write_callback_mock  callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;
                sender.submit(
                    write_token{error_mock, cancel_mock, callback_mock});

                WHEN("The action callback is invoked")
                {
                    write_token writer_token;
                    REQUIRE_CALL(writer.sender_, submit(ANY(write_token)))
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

SCENARIO("Const action adaptor.")
{
    action_mock closure;
    GIVEN("Writer")
    {
        write_mock writer;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = action(writer, closure);
        }
        THEN("A constant piped adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = writer | action(closure);
        }
    }

    GIVEN("Reader")
    {
        read_mock reader;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = action(reader, closure);
        }
        THEN("A constant piped adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = reader | action(closure);
        }
    }
}

SCENARIO("R-value writer and callback.")
{
    action_mock           closure;
    [[maybe_unused]] auto s = action(move_only_reader{}, closure);
}

SCENARIO("Pipe operator")
{
    action_mock closure;
    write_mock  writer;

    [[maybe_unused]] auto s = writer | action(closure);
}

SCENARIO("Double action")
{
    GIVEN("A write stream.")
    {
        write_mock  writer;
        action_mock closure1;
        action_mock closure2;

        auto s = action(action(writer, closure1), closure2);

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            REQUIRE_CALL(closure1, call()).LR_RETURN(closure1.sender_);
            REQUIRE_CALL(closure2, call()).LR_RETURN(closure2.sender_);
            auto sender = s.write(2);

            WHEN("Synchronous submit is called on the sender.")
            {
                trompeloeil::sequence seq;
                REQUIRE_CALL(closure2.sender_, submit()).IN_SEQUENCE(seq);
                REQUIRE_CALL(closure1.sender_, submit()).IN_SEQUENCE(seq);
                ALLOW_CALL(writer.sender_, submit()).IN_SEQUENCE(seq);
                sender.submit();
            }
        }
    }
}

SCENARIO("Contiguous range is preserved.")
{
    GIVEN("A write stream.")
    {
        action_mock closure;
        write_mock  writer{true};
        ALLOW_CALL(writer, write_(ANY(vector<int>)));
        REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);

        auto s = action(writer, closure);

        WHEN("With a BidirectionalRange.")
        {
            REQUIRE_CALL(writer, bidirectional_write_());
            list<int> l;
            s.write(l);
        }

        WHEN("With a ContiguousRange.")
        {
            REQUIRE_CALL(writer, contiguous_write_());
            vector<int> v;
            s.write(v);
        }
    }
}
