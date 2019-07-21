/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include "action.hpp"

#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>

using namespace stream;
using namespace std;
namespace ranges = std::experimental::ranges;

SCENARIO("Actions with single values.")
{
    GIVEN("A stream that increments a variable for each write.")
    {
        write_stream ws;
        char         counter = 0;
        auto         s       = action(ws, [&]() { ++counter; });

        WHEN("A single value is written.")
        {
            s.write(2).submit();
            THEN("The value is written.") { REQUIRE(ws.v_ == 2); }
            THEN("The counter is incremented by one.")
            {
                REQUIRE(counter == 1);
            }
        }

        WHEN("A single value is written asynchronously.")
        {
            auto callback = [&](auto ec) {
                THEN("The value is written.") { REQUIRE(ws.v_ == 3); }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The counter is incremented by one.")
                {
                    REQUIRE(counter == 1);
                }
            };
            auto sender = s.write(3);
            THEN("Before submit the counter is not incremented.")
            {
                REQUIRE(counter == 0);
            }
            sender.submit(callback);
        }
    }
}

SCENARIO("Actions with ranges.")
{
    GIVEN("A stream that increments a variable for each write.")
    {
        write_stream ws;
        char         counter = 0;
        auto         s       = action(ws, [&]() { ++counter; });

        WHEN("[2, 3] is written.")
        {
            s.write(array{2, 3}).submit();
            THEN("[2, 3] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
            THEN("The counter is incremented by one.")
            {
                REQUIRE(counter == 1);
            }
        }

        WHEN("[3, 4] is written asynchronously.")
        {
            auto callback = [&](auto ec, auto n) {
                THEN("[3, 4] is written.")
                {
                    REQUIRE(ranges::equal(ws.vs_, array{3, 4}));
                }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 2.")
                {
                    REQUIRE(n == 2);
                }
                THEN("The counter is incremented by one.")
                {
                    REQUIRE(counter == 1);
                }
            };
            auto sender = s.write(array{3, 4});
            sender.submit(callback);
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            s.write(ranges::view::iota(0, 3)).submit();
            THEN("[0, 1, 2] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{0, 1, 2}));
            }
            THEN("The counter is incremented by exactly one.")
            {
                REQUIRE(counter == 1);
            }
        }
    }
}
