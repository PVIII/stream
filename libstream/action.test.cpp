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
            auto sender = s.write(2);
            ALLOW_CALL(closure, call());

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure, call());
                test_sync_submit(writer.sender_, sender);
                test_async_write_submit(writer.sender_, sender);
                test_async_write_submit(writer.sender_, sender, dummy_error);
            }
        }

        WHEN("Range write is called.")
        {
            REQUIRE_CALL(writer, write_(vector{2, 3}));
            auto sender = s.write(array{2, 3});
            ALLOW_CALL(closure, call());

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure, call());

                test_sync_submit(writer.range_sender_, sender);
                test_async_range_submit(writer.range_sender_, sender, {2, 2});
                test_async_range_submit(writer.range_sender_, sender, {2, 2},
                                        dummy_error);
            }
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            REQUIRE_CALL(writer, write_(vector{0, 1, 2}));
            auto sender = s.write(ranges::view::iota(0, 3));
            ALLOW_CALL(closure, call());

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure, call());

                test_sync_submit(writer.range_sender_, sender);
                test_async_range_submit(writer.range_sender_, sender, {3, 3});
                test_async_range_submit(writer.range_sender_, sender, {3, 3},
                                        dummy_error);
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
            auto sender = s.read();
            ALLOW_CALL(closure, call());

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure, call());

                test_sync_read_submit(reader.sender_, sender, test_pair{1, 1});
                test_async_read_submit(reader.sender_, sender, test_pair{1, 1});
                test_async_read_submit(reader.sender_, sender, test_pair{1, 1},
                                       dummy_error);
            }
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{2, 3});
            array a{0, 0};
            auto  sender = s.read(a);
            REQUIRE_THAT(a, Equals(array{2, 3}));
            ALLOW_CALL(closure, call());

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure, call());

                test_sync_submit(reader.range_sender_, sender);
                test_async_range_submit(reader.range_sender_, sender,
                                        test_pair{2, 2});
                test_async_range_submit(reader.range_sender_, sender,
                                        test_pair{2, 2}, dummy_error);
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
            auto sender = s.readwrite(1);
            ALLOW_CALL(closure, call());

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure, call());

                test_sync_read_submit(readwriter.sender_, sender,
                                      test_pair{1, 1});
                test_async_read_submit(readwriter.sender_, sender,
                                       test_pair{1, 1});
                test_async_read_submit(readwriter.sender_, sender,
                                       test_pair{1, 1}, dummy_error);
            }
        }

        WHEN("Ranges are read and written.")
        {
            REQUIRE_CALL(readwriter, readwrite_(vector{2, 3}, _))
                .SIDE_EFFECT(_2 = vector{4, 5});
            array a{0, 0};
            auto  sender = s.readwrite(array{2, 3}, a);
            REQUIRE_THAT(a, Equals(array{4, 5}));
            ALLOW_CALL(closure, call());

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure, call());

                test_sync_submit(readwriter.range_sender_, sender);
                test_async_range_submit(readwriter.range_sender_, sender,
                                        {2, 2});
                test_async_range_submit(readwriter.range_sender_, sender,
                                        {2, 2}, dummy_error);
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
            auto sender = s.write(1);
            ALLOW_CALL(closure, call());

            WHEN("The operation is cancelled.")
            {
                write_token t;
                ALLOW_CALL(writer.sender_, submit(ANY(write_token)))
                    .LR_SIDE_EFFECT(t = _1;);
                cancel_callback_mock cancel_mock;

                sender.submit(write_token{error_callback_mock{}, cancel_mock,
                                          write_callback_mock{}});
                REQUIRE_CALL(writer.sender_, cancel());
                sender.cancel();

                REQUIRE_CALL(cancel_mock, call());
                t.cancelled();
            }
        }
    }
}

SCENARIO("Const action adaptor.")
{
    GIVEN("Writer")
    {
        write_mock writer;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = action(writer, [] {});
        }
        THEN("A constant piped adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = writer | action([] {});
        }
    }

    GIVEN("Reader")
    {
        read_mock reader;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = action(reader, [] {});
        }
        THEN("A constant piped adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s = reader | action([] {});
        }
    }
}

SCENARIO("R-value writer and callback.")
{
    [[maybe_unused]] auto s = action(move_only_reader{}, [] {});
}

SCENARIO("Pipe operator")
{
    write_mock writer;

    [[maybe_unused]] auto s = writer | action([] {});
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
            auto sender = s.write(2);

            WHEN("Synchronous submit is called on the sender.")
            {
                trompeloeil::sequence seq;
                REQUIRE_CALL(closure2, call()).IN_SEQUENCE(seq);
                REQUIRE_CALL(closure1, call()).IN_SEQUENCE(seq);
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
        write_mock writer{true};
        ALLOW_CALL(writer, write_(ANY(vector<int>)));

        auto s = action(writer, [] {});

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
