/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/filter.hpp>

#include <tests/helpers/submit_tester.hpp>
#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

using namespace stream;
using namespace std;

SCENARIO("Normal submits.")
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
}
