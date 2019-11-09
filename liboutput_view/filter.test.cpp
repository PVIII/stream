/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <liboutput_view/filter.hpp>

#include <catch2/catch.hpp>

#include <array>
#include <experimental/ranges/algorithm>

namespace ranges = std::experimental::ranges;
using std::array;

SCENARIO("Assignments.")
{
    GIVEN("An adaptor that filters fives.")
    {
        array a{0};
        auto  v = output_view::filter(a, [](auto v) { return v != 5; });

        WHEN("Five is assigned.")
        {
            *begin(v) = 5;
            THEN("Nothing is stored.") { REQUIRE(a[0] == 0); }
        }
        WHEN("One is assigned.")
        {
            *begin(v) = 1;
            THEN("One is assigned.") { REQUIRE(a[0] == 1); }
        }

        GIVEN("An additional adaptor filtering threes.")
        {
            auto v2 = output_view::filter(v, [](auto v) { return v != 3; });

            WHEN("Three is assigned.")
            {
                *begin(v2) = 3;
                THEN("Nothing is stored.") { REQUIRE(a[0] == 0); }
            }
            WHEN("Five is assigned.")
            {
                *begin(v2) = 5;
                THEN("Nothing is stored.") { REQUIRE(a[0] == 0); }
            }
            WHEN("one is assigned.")
            {
                *begin(v2) = 1;
                THEN("One is stored.") { REQUIRE(a[0] == 1); }
            }
        }
    }

    GIVEN("An array.")
    {
        array a{0, 0, 0};
        auto  v = output_view::filter(a, [](auto v) { return v != 5; });

        WHEN("[0, 5, 1, 5, 0] is copied.")
        {
            ranges::copy(array{0, 5, 1, 5, 0}, begin(v));
            THEN("[0, 1, 0] is stored.")
            {
                REQUIRE(ranges::equal(a, array{0, 1, 0}));
            }
        }
    }
}

SCENARIO("Passing the adapter.")
{
    GIVEN("An adaptor.")
    {
        auto  f = []([[maybe_unused]] ranges::Range&& r) {};
        array a{0};
        auto  r = output_view::filter(
            a, []([[maybe_unused]] auto v) { return true; });

        THEN("The adaptor can be passed as a range.") { f(r); }
    }
}
