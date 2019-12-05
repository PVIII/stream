/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/take_until.hpp>

#include <tests/helpers/range_matcher.hpp>
#include <tests/helpers/submit_tester.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/readwritestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

using namespace stream;
using namespace std;
using trompeloeil::_;

SCENARIO("Reads.")
{
    GIVEN("A stream that reads until 1.")
    {
        read_mock reader;
        auto s = stream::take_until_read(reader, [](auto v) { return v == 1; });

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{3, 1, 2});
            array a{0, 0, 0};
            auto  sender = s.read(a);
            REQUIRE_THAT(a, Equals(array{3, 1, 0}));

            test_sync_submit(reader.range_sender_, sender);
            test_async_range_submit(reader.range_sender_, sender);
            test_async_range_submit(reader.range_sender_, sender, dummy_error);
        }
    }

    GIVEN("A read-write stream that reads until 1.")
    {
        read_write_mock readwriter;
        auto            s =
            stream::take_until_read(readwriter, [](auto v) { return v == 1; });

        WHEN("A range is read.")
        {
            REQUIRE_CALL(readwriter, readwrite_(vector{0, 1, 2}, _))
                .SIDE_EFFECT(_2 = vector{3, 1, 2});
            array a_read{0, 0, 0};
            array a_write{0, 1, 2};
            auto  sender = s.readwrite(a_write, a_read);

            test_sync_submit(readwriter.range_sender_, sender);
            test_async_range_submit(readwriter.range_sender_, sender);
            test_async_range_submit(readwriter.range_sender_, sender,
                                    dummy_error);
        }
    }
}

SCENARIO("Cancelling operations.")
{
    GIVEN("A stream that reads until 1.")
    {
        read_mock reader;
        auto s = stream::take_until_read(reader, [](auto v) { return v == 1; });

        WHEN("Range asynchronous read is called.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{3, 1, 2});

            done_callback_mock   callback_mock;
            error_callback_mock  error_mock;
            cancel_callback_mock cancel_mock;
            array                a{0, 0, 0};
            auto                 sender = s.read(a);

            base_token t;
            REQUIRE_CALL(reader.range_sender_, submit(ANY(base_token)))
                .LR_SIDE_EFFECT(t = _1);
            sender.submit(base_token{error_mock, cancel_mock, callback_mock});

            WHEN("The operation is cancelled.")
            {
                REQUIRE_CALL(reader.range_sender_, cancel());
                sender.cancel();

                WHEN("The cancel callback is invoked.")
                {
                    REQUIRE_CALL(cancel_mock, call());
                    t.cancelled();
                }
            }
        }
    }
}
