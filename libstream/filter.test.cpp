/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/filter.hpp>

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
}
