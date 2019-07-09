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
            s.write(1);
            THEN("Two is stored.") { REQUIRE(ws.v_ == 2); }
        }

        WHEN("One is written asynchronously.")
        {
            s.write(1, [](auto ec, auto n) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 1.")
                {
                    REQUIRE(n == 1);
                }
            });
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
            THEN("The transformed stream reads 2.") { REQUIRE(s.read() == 2); }
        }

        WHEN("One is read asynchronously.")
        {
            rs.v_         = 1;
            auto callback = [](auto ec, auto v) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The returned value is 2.") { REQUIRE(v == 2); }
            };
            s.read(callback);
            rs.read_callback();
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
            s.write(array{1, 2});
            THEN("[2, 3] is sent.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
        }

        WHEN("[1, 2] is written asynchronously.")
        {
            s.write(array{1, 2}, [](auto ec, auto n) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 2.")
                {
                    REQUIRE(n == 2);
                }
            });
            ws.callback();
            THEN("[2, 3] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
        }

        WHEN("[1, 2, 3] is generated and written.")
        {
            s.write(ranges::view::iota(1, 4));
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
            auto               n = s.read(a);
            THEN("Two elements have been read.") { REQUIRE(n == 2); }
            THEN("The transformed stream reads [2, 3].")
            {
                REQUIRE_THAT(a, Equals(array{2, 3}));
            }
        }

        WHEN("The stream reads [1, 2] asynchronously.")
        {
            rs.vs_ = {1, 2};
            std::array<int, 2> a;
            auto               callback = [&](auto ec, auto n) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("2 values have been read.") { REQUIRE(n == 2); }
                THEN("The transformed stream reads [2, 3].")
                {
                    REQUIRE_THAT(a, Equals(array{2, 3}));
                }
            };
            auto context = s.make_read_context(a);
            s.read(a, callback, &context);
            rs.do_read<decltype(context)::inner_type>();
            rs.range_callback();
        }
    }
}
