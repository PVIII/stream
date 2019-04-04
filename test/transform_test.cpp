#include "stream/transform.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <array>
#include <experimental/ranges/algorithm>

#include "mocks/writestream.h"

using namespace stream;
using namespace std;
using namespace experimental;

SCENARIO("Writing single values.")
{
    GIVEN("A write stream that adds one.")
    {
        write_stream ws;
        auto         s = stream::transform(ws, [](char v) { return v + 1; });

        WHEN("One is written.")
        {
            s.write(1);
            THEN("Two is stored.") { REQUIRE(ws.v_ == 2); }
        }
    }
}

SCENARIO("Writing a range.")
{
    GIVEN("A write stream that adds 1 to a range.")
    {
        write_stream ws;
        auto         s = stream::transform(ws, [](auto const& r) {
            vector<char> result;
            ranges::transform(r, ranges::back_inserter(result),
                              [](auto v) { return v + 1; });
            return result;
        });
        WHEN("[1, 2] is written.")
        {
            s.write(array{1, 2});
            THEN("[3, 4] is sent.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
        }
    }
}

SCENARIO("Changing the type of the data.")
{
    GIVEN("A write stream that writes the size of the container.")
    {
        write_stream ws;
        auto         s = stream::transform(ws, [](auto r) { return r.size(); });

        WHEN("An array with two elements in written.")
        {
            s.write(array{1, 2});
            THEN("Two is sent.") { REQUIRE(ws.v_ == 2); }
        }
    }
}
