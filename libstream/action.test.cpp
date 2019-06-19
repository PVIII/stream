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
            s.write(2);
            THEN("The value is written.") { REQUIRE(ws.v_ == 2); }
            THEN("The counter is incremented.") { REQUIRE(counter == 1); }
        }

        WHEN("A single value is written asynchronously.")
        {
            s.write(3, [&](auto ec, auto n) {
                THEN("The value is written.") { REQUIRE(ws.v_ == 3); }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 1.")
                {
                    REQUIRE(n == 1);
                }
            });
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
            s.write(array{2, 3});
            THEN("[2, 3] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
            THEN("The counter is incremented by exactly one.")
            {
                REQUIRE(counter == 1);
            }
        }

        WHEN("[3, 4] is written asynchronously.")
        {
            s.write(array{3, 4}, [&](auto ec, auto n) {
                THEN("[3, 4] is written.")
                {
                    REQUIRE(ranges::equal(ws.vs_, array{3, 4}));
                }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 1.")
                {
                    REQUIRE(n == 2);
                }
            });
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            s.write(ranges::view::iota(0, 3));
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
