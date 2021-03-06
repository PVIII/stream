/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <liboutput_view/transform.hpp>

#include <catch2/catch.hpp>

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/iterator>

namespace ranges = std::experimental::ranges;
using std::array;

SCENARIO("Assignments.")
{
    GIVEN("An array with one element.")
    {
        array a{0};
        auto  v = output_view::transform(a, [](auto v) { return v + 1; });

        WHEN("Zero is assigned.")
        {
            *begin(v) = 0;
            THEN("One is stored.") { REQUIRE(a[0] == 1); }
        }
    }

    GIVEN("An array with two elements.")
    {
        array a{0, 0};
        auto  v = output_view::transform(a, [](auto v) { return v + 1; });

        WHEN("[0, 1] is copied.")
        {
            ranges::copy(array{0, 1}, begin(v));
            THEN("[1, 2] is stored.")
            {
                REQUIRE(ranges::equal(a, array{1, 2}));
            }
        }
    }
}
