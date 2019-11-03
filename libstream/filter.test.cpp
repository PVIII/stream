/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/filter.hpp>

#include <tests/helpers/constrained_types.hpp>
#include <tests/helpers/range_matcher.hpp>
#include <tests/helpers/submit_tester.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/readwritestream.hpp>
#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

using namespace stream;
using namespace std;

SCENARIO("Filter writes.")
{
    GIVEN("A write stream that filters 0.")
    {
        write_mock writer;
        auto s = stream::filter_write(writer, [](auto v) { return v != 0; });

        WHEN("Zero is written.")
        {
            auto sender = s.write(0);
            WHEN("Synchronous submit is called.") { sender.submit(); }
            WHEN("Asynchronous submit is called.")
            {
                write_callback_mock  callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;

                REQUIRE_CALL(callback_mock, call());
                sender.submit(
                    write_token{error_mock, cancel_mock, callback_mock});
            }
        }
        WHEN("One is written.")
        {
            REQUIRE_CALL(writer, write(1)).LR_RETURN(writer.sender_);
            auto sender = s.write(1);

            test_sync_submit(writer.sender_, sender);
            test_async_write_submit(writer.sender_, sender);
            test_async_write_submit(writer.sender_, sender, 1);
        }

        WHEN("A range is written.")
        {
            REQUIRE_CALL(writer, write_(vector{1, 2}));
            array a{0, 1, 2, 0};
            auto  sender = s.write(a);

            test_sync_submit(writer.range_sender_, sender);
            test_async_range_submit(writer.range_sender_, sender, {2, 2});
            test_async_range_submit(writer.range_sender_, sender, {2, 2},
                                    dummy_error);
        }
    }

    GIVEN("A read-write stream that filters 0.")
    {
        read_write_mock readwriter;
        auto            s =
            stream::filter_write(readwriter, [](auto v) { return v != 0; });

        WHEN("A range is written.")
        {
            REQUIRE_CALL(readwriter, readwrite_(vector{1, 2}, _))
                .SIDE_EFFECT(_2 = vector{0, 1});
            array a_read{0, 0};
            array a_write{0, 1, 2, 0};
            auto  sender = s.readwrite(a_write, a_read);
            REQUIRE_THAT(a_read, Equals(array{0, 1}));

            test_sync_submit(readwriter.range_sender_, sender);
            test_async_range_submit(readwriter.range_sender_, sender, {3, 3});
            test_async_range_submit(readwriter.range_sender_, sender, {3, 3},
                                    dummy_error);
        }
    }
}

SCENARIO("Filter reads.")
{
    GIVEN("A read stream that filters 0.")
    {
        read_mock reader;
        auto s = stream::filter_read(reader, [](auto v) { return v != 0; });

        WHEN("Zero is read.")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
            auto sender = s.read();

            WHEN("Synchronous submit is called.")
            {
                int i = 0;
                REQUIRE_CALL(reader.sender_, submit())
                    .LR_RETURN(i)
                    .LR_SIDE_EFFECT(++i);
                REQUIRE_CALL(reader.sender_, submit()).LR_RETURN(i);
                REQUIRE(sender.submit() == 1);
            }
            WHEN("Asynchronous submit is called.")
            {
                read_token<int> t;
                REQUIRE_CALL(reader.sender_, submit(ANY(read_token<int>)))
                    .LR_SIDE_EFFECT(t = _1);
                read_callback_mock   callback_mock;
                error_callback_mock  error_mock;
                cancel_callback_mock cancel_mock;

                sender.submit(
                    read_token<int>{error_mock, cancel_mock, callback_mock});

                WHEN("The callback is invoked with 0.")
                {
                    REQUIRE_CALL(reader.sender_, submit(ANY(read_token<int>)))
                        .LR_SIDE_EFFECT(t = _1);
                    t.done(0);

                    WHEN("The callback is invoked with 1.")
                    {
                        REQUIRE_CALL(callback_mock, call(1));
                        t.done(1);
                    }
                }
            }
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{1, 0, 2});
            array a{0, 0};
            auto  sender = s.read(a);
            REQUIRE_THAT(a, Equals(array{1, 2}));

            test_sync_submit(reader.range_sender_, sender);
            test_async_range_submit(reader.range_sender_, sender, {2, 2});
            test_async_range_submit(reader.range_sender_, sender, {2, 2},
                                    dummy_error);
        }
    }

    GIVEN("A read-write stream that filters 0.")
    {
        read_write_mock readwriter;
        auto s = stream::filter_read(readwriter, [](auto v) { return v != 0; });

        WHEN("A range is read.")
        {
            REQUIRE_CALL(readwriter, readwrite_(vector{0, 1, 2, 0}, _))
                .SIDE_EFFECT(_2 = vector{0, 3, 0, 4});
            array a_read{0, 0};
            array a_write{0, 1, 2, 0};
            auto  sender = s.readwrite(a_write, a_read);
            REQUIRE_THAT(a_read, Equals(array{3, 4}));

            test_sync_submit(readwriter.range_sender_, sender);
            test_async_range_submit(readwriter.range_sender_, sender, {2, 2});
            test_async_range_submit(readwriter.range_sender_, sender, {2, 2},
                                    dummy_error);
        }
    }
}

SCENARIO("Cancelling operations.")
{
    GIVEN("A write stream that adds one.")
    {
        write_mock writer;
        auto s = stream::filter_write(writer, [](auto v) { return v != 0; });

        WHEN("Single value asynchronous write is called.")
        {
            REQUIRE_CALL(writer, write(1)).LR_RETURN(writer.sender_);

            write_callback_mock  callback_mock;
            error_callback_mock  error_mock;
            cancel_callback_mock cancel_mock;
            auto                 sender = s.write(1);

            write_token writer_token;
            REQUIRE_CALL(writer.sender_, submit(ANY(write_token)))
                .LR_SIDE_EFFECT(writer_token = _1);
            sender.submit(write_token{error_mock, cancel_mock, callback_mock});

            WHEN("The operation is cancelled.")
            {
                REQUIRE_CALL(writer.sender_, cancel());
                sender.cancel();

                WHEN("The cancel callback is invoked.")
                {
                    REQUIRE_CALL(cancel_mock, call());
                    writer_token.cancelled();
                }
            }
        }
    }

    GIVEN("A read stream that adds one.")
    {
        read_mock reader;
        auto s = stream::filter_read(reader, [](auto v) { return v != 1; });

        WHEN("Single value asynchronous read is called.")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);

            read_callback_mock   callback_mock;
            error_callback_mock  error_mock;
            cancel_callback_mock cancel_mock;
            auto                 sender = s.read();

            read_token<int> reader_token;
            REQUIRE_CALL(reader.sender_, submit(ANY(read_token<int>)))
                .LR_SIDE_EFFECT(reader_token = _1);
            sender.submit(
                read_token<int>{error_mock, cancel_mock, callback_mock});

            WHEN("The operation is cancelled.")
            {
                REQUIRE_CALL(reader.sender_, cancel());
                sender.cancel();

                WHEN("The cancel callback is invoked.")
                {
                    REQUIRE_CALL(cancel_mock, call());
                    reader_token.cancelled();
                }
            }
        }
    }
}

SCENARIO("Const filter adaptor")
{
    GIVEN("Writer")
    {
        write_mock writer;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s =
                stream::filter_write(writer, [](auto v) { return v != 0; });
        }
    }

    GIVEN("Reader")
    {
        read_mock reader;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s =
                stream::filter_read(reader, [](auto v) { return v != 0; });
        }
    }
}

SCENARIO("R-value reader and callback")
{
    [[maybe_unused]] auto s =
        stream::filter_read(move_only_reader{}, [](int v) { return v != 0; });
}

SCENARIO("Pipe operator")
{
    write_mock writer;
    writer | stream::filter_write([](int v) { return v != 0; });

    read_mock reader;
    reader | stream::filter_read([](int v) { return v != 0; });
}

SCENARIO("Double filter")
{
    GIVEN("A write stream.")
    {
        write_mock writer;

        auto s = stream::filter_write(
            stream::filter_write(writer, [](auto v) { return v >= 1; }),
            [](auto v) { return v <= 4; });

        WHEN("0 is written.") { s.write(0).submit(); }
        WHEN("5 is written.") { s.write(5).submit(); }
        WHEN("3 is written.")
        {
            REQUIRE_CALL(writer, write(3)).LR_RETURN(writer.sender_);
            auto sender = s.write(3);
            test_sync_submit(writer.sender_, sender);
        }
        WHEN("[0, 5, 3] is written.")
        {
            REQUIRE_CALL(writer, write_(vector{3}));
            REQUIRE_CALL(writer.range_sender_, submit());
            array a{0, 5, 3};
            s.write(a).submit();
        }
    }
    GIVEN("A read stream.")
    {
        read_mock reader;

        auto s = stream::filter_read(
            stream::filter_read(reader, [](auto v) { return v >= 1; }),
            [](auto v) { return v <= 4; });

        REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
        auto sender = s.read();

        WHEN("0 is read.")
        {
            int i = 0;
            REQUIRE_CALL(reader.sender_, submit())
                .LR_RETURN(i)
                .LR_SIDE_EFFECT(++i);
            REQUIRE_CALL(reader.sender_, submit()).LR_RETURN(i);
            REQUIRE(sender.submit() == 1);
        }
        WHEN("5 is read.")
        {
            int i = 5;
            REQUIRE_CALL(reader.sender_, submit())
                .LR_RETURN(i)
                .LR_SIDE_EFFECT(--i);
            REQUIRE_CALL(reader.sender_, submit()).LR_RETURN(i);
            REQUIRE(sender.submit() == 4);
        }
        WHEN("3 is read.")
        {
            REQUIRE_CALL(reader.sender_, submit()).LR_RETURN(3);
            REQUIRE(sender.submit() == 3);
        }
        WHEN("[0, 5, 3] is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{0, 5, 3});
            REQUIRE_CALL(reader.range_sender_, submit());
            array a{0};
            s.read(a).submit();
            REQUIRE_THAT(a, Equals(array{3}));
        }
    }
}
