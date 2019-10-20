/*
 * filter.test.cpp
 *
 *  Created on: Oct 20, 2019
 *      Author: patrick
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
    }

    GIVEN("An array with two elements.")
    {
        array a{0, 0};
        auto  v = output_view::filter(a, [](auto v) { return v != 5; });

        WHEN("[0, 5, 1,] is copied.")
        {
            ranges::copy(array{0, 5, 1}, begin(v));
            THEN("[0, 1] is stored.")
            {
                REQUIRE(ranges::equal(a, array{0, 1}));
            }
        }
    }
}
