/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/pipe.hpp>

#include <libstream/action.hpp>
#include <libstream/transform.hpp>

#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

using namespace stream;
using namespace std;
using trompeloeil::_;

struct action_mock
{
    void operator()() const { return call(); }
    MAKE_CONST_MOCK0(call, void());
};

SCENARIO("Piping without stream.")
{
    action_mock closure;

    auto p = stream::transform([](auto v) { return v + 1; }) |
             stream::action(closure);

    GIVEN("A write stream.")
    {
        write_mock writer;

        auto s = writer | p;

        WHEN("Single write is called")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            auto sender = s.write(1);
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called on the sender.")
            {
                REQUIRE_CALL(writer.sender_, submit());
                sender.submit();
            }
        }
    }
}
