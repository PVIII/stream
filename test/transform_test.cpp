#include "stream/transform.h"

#include <catch2/catch.hpp>

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>

#include "mocks/writestream.h"

using namespace stream;
using namespace std;
using namespace experimental;

SCENARIO("Transformations with single values.")
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

        WHEN("One is written asynchronously.")
        {
            s.write(1, [&](auto ec, auto n) {
                THEN("Two is stored.") { REQUIRE(ws.v_ == 2); }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 1.")
                {
                    REQUIRE(n == 1);
                }
            });
        }
    }
}

SCENARIO("Transformations with ranges.")
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

        WHEN("[1, 2] is written asynchronously.")
        {
            s.write(array{1, 2}, [&](auto ec, auto n) {
                THEN("[3, 4] is written.")
                {
                    REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
                }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 2.")
                {
                    REQUIRE(n == 2);
                }
            });
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            s.write(ranges::view::iota(0, 3));
            THEN("[1, 2, 3] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{1, 2, 3}));
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
            THEN("2 is written.") { REQUIRE(ws.v_ == 2); }
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            s.write(ranges::view::iota(0, 3));
            THEN("3 is written.") { REQUIRE(ws.v_ == 3); }
        }
    }
}
