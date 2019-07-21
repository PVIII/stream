/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/transform.hpp>

#include <catch2/catch.hpp>

#include <tests/helpers/range_matcher.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/writestream.hpp>

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>

using namespace stream;
using namespace std;
namespace ranges = std::experimental::ranges;

SCENARIO("Transformations with single values.")
{
    GIVEN("A write stream that adds one.")
    {
        write_stream ws;
        auto         s = stream::transform(ws, [](auto v) { return v + 1; });

        WHEN("One is written.")
        {
            s.write(1).submit();
            THEN("Two is stored.") { REQUIRE(ws.v_ == 2); }
        }

        WHEN("One is written asynchronously.")
        {
            auto sender   = s.write(1);
            auto callback = [](auto ec) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
            };
            sender.submit(callback);
            ws.callback();
            THEN("Two is stored.") { REQUIRE(ws.v_ == 2); }
        }
    }

    GIVEN("A read stream that adds one.")
    {
        read_stream rs;
        auto        s = stream::transform(rs, [](auto v) { return v + 1; });

        WHEN("The read stream reads 1.")
        {
            rs.v_ = 1;
            THEN("The transformed stream reads 2.")
            {
                REQUIRE(s.read<char>().submit() == 2);
            }
        }

        WHEN("One is read asynchronously.")
        {
            auto callback = [](auto ec, auto v) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The returned value is 2.") { REQUIRE(v == 2); }
            };
            auto sender = s.read<char>();
            sender.submit(callback);
            rs.read_callback(1);
        }
    }
}

SCENARIO("Transformations with ranges.")
{
    GIVEN("A write stream that adds 1 to a range.")
    {
        write_stream ws;
        auto         s = stream::transform(ws, [](auto v) { return v + 1; });
        WHEN("[1, 2] is written.")
        {
            auto a = array{1, 2};
            s.write(a).submit();
            THEN("[2, 3] is sent.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
        }

        WHEN("[1, 2] is written asynchronously.")
        {
            auto callback = [](auto ec, auto n) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 2.")
                {
                    REQUIRE(n == 2);
                }
            };
            auto a      = array{1, 2};
            auto sender = s.write(a);
            sender.submit(callback);
            ws.range_callback();
            THEN("[2, 3] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
        }

        WHEN("[1, 2, 3] is generated and written.")
        {
            s.write(ranges::view::iota(1, 4)).submit();
            THEN("[2, 3, 4] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3, 4}));
            }
        }
    }

    GIVEN("A read stream that adds one to the range.")
    {
        read_stream rs;
        auto        s = stream::transform(rs, [](auto v) { return v + 1; });
        WHEN("The read stream reads [1, 2].")
        {
            rs.vs_ = {1, 2};
            std::array<int, 2> a;
            auto               n = s.read(a).submit();
            THEN("Two elements have been read.") { REQUIRE(n == 2); }
            THEN("The transformed stream reads [2, 3].")
            {
                REQUIRE_THAT(a, Equals(array{2, 3}));
            }
        }

        WHEN("The stream reads [1, 2] asynchronously.")
        {
            rs.vs_ = {1, 2};
            std::array<int, 2> a{0, 0};
            auto               callback = [&](auto ec, auto n) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("2 values have been read.") { REQUIRE(n == 2); }
                THEN("The transformed stream reads [2, 3].")
                {
                    REQUIRE_THAT(a, Equals(array{2, 3}));
                }
            };
            auto sender = s.read(a);
            THEN("Nothing has happened before submit.")
            {
                REQUIRE_THAT(a, Equals(array{0, 0}));
            }
            sender.submit(callback);
            rs.range_callback();
        }
    }
}
