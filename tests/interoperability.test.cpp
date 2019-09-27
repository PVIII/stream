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

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure.sender_, submit());

                test_sync_submit(writer.sender_, sender);
                test_async_write_submit(writer.sender_, sender);
                test_async_write_submit(writer.sender_, sender, 1);
            }
        }

        WHEN("Range write is called")
        {
            REQUIRE_CALL(writer, write_(vector{2, 3}));
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);

            auto a      = array{1, 2};
            auto sender = s2.write(a);

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure.sender_, submit());

                test_sync_submit(writer.range_sender_, sender);
                test_async_range_submit(writer.range_sender_, sender, {2, 2});
                test_async_range_submit(writer.range_sender_, sender, {2, 2},
                                        dummy_error);
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

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure.sender_, submit());

                test_sync_read_submit(reader.sender_, sender, test_pair{1, 2});
                test_async_read_submit(reader.sender_, sender, test_pair{1, 2});
                test_async_read_submit(reader.sender_, sender, test_pair{1, 2},
                                       dummy_error);
            }
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{1, 2});
            REQUIRE_CALL(closure, call()).LR_RETURN(closure.sender_);

            array<int, 2> a;
            auto sender = s2.read(a);
            REQUIRE_THAT(a, Equals(array{2, 3}));

            WHEN("The operation is not cancelled.")
            {
                REQUIRE_CALL(closure.sender_, submit());

                test_sync_submit(reader.range_sender_, sender);
                test_async_range_submit(reader.range_sender_, sender, {2, 2});
                test_async_range_submit(reader.range_sender_, sender, {2, 2},
                                        dummy_error);
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

            WHEN("The operation is cancelled.")
            {
                write_token t;
                ALLOW_CALL(writer.sender_, submit(ANY(write_token)))
                    .LR_SIDE_EFFECT(t = _1;);
                REQUIRE_CALL(closure.sender_, submit());

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
