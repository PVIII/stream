/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <liboutput_view/take_while.hpp>

#include <tests/helpers/range_matcher.hpp>

#include <catch2/catch.hpp>

#include <array>
#include <experimental/ranges/algorithm>

using namespace stream;
using namespace std::experimental::ranges;
using std::array;

SCENARIO("Assignments.")
{
    GIVEN("An adaptor that writes until 2.")
    {
        array a{0, 0, 0};
        auto  v = output_view::take_while(a, [](auto v) { return v != 2; });

        WHEN("[1, 2, 3] is copied.")
        {
            ext::copy(array{1, 2, 3}, v);
            THEN("[1] is stored.") { REQUIRE_THAT(a, Equals(array{1, 0, 0})); }
        }
        WHEN("[1, 3, 4] is copied.")
        {
            ext::copy(array{1, 3, 4}, v);
            THEN("[1, 3, 4] is stored.")
            {
                REQUIRE_THAT(a, Equals(array{1, 3, 4}));
            }
        }
        WHEN("[2] is copied.")
        {
            ext::copy(array{2}, v);
            THEN("[] is stored.") { REQUIRE_THAT(a, Equals(array{0, 0, 0})); }
        }
        WHEN("[2, 3, 4] is copied.")
        {
            ext::copy(array{2, 3, 4}, v);
            THEN("[] is stored.") { REQUIRE(equal(a, array{0, 0, 0})); }
        }

        GIVEN("An additional adaptor writing until 3.")
        {
            auto v2 = output_view::take_while(v, [](auto i) { return i != 3; });

            WHEN("[1, 2, 3] is copied.")
            {
                ext::copy(array{1, 2, 3}, v2);
                THEN("[1] is stored.") { REQUIRE(equal(a, array{1, 0, 0})); }
            }
            WHEN("[1, 3, 2] is copied.")
            {
                ext::copy(array{1, 3, 2}, v2);
                THEN("[1] is stored.") { REQUIRE(equal(a, array{1, 0, 0})); }
            }
        }
    }
}

SCENARIO("Passing the adapter.")
{
    GIVEN("An adaptor.")
    {
        auto  f = []([[maybe_unused]] Range&& r) {};
        array a{0};
        auto  r = output_view::take_while(
            a, []([[maybe_unused]] auto v) { return true; });

        THEN("The adaptor can be passed as a range.") { f(r); }
    }
}
