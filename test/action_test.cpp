#include "mocks/writestream.h"
#include "stream/action.h"

#include <catch2/catch.hpp>

#include <array>
#include <experimental/ranges/algorithm>

using namespace stream;
using namespace std;
using namespace experimental;

SCENARIO("A simple action before write.")
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

        WHEN("A range is written.")
        {
            s.write(array{2, 3});
            THEN("The range is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
            THEN("The counter is incremented by exactly one.")
            {
                REQUIRE(counter == 1);
            }
        }
    }
}
