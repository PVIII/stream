/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include "action.hpp"

#include <tests/helpers/constrained_types.hpp>
#include <tests/helpers/range_matcher.hpp>
#include <tests/mocks/callback.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>

using namespace stream;
using namespace std;
namespace ranges = std::experimental::ranges;
using trompeloeil::_;

struct action_mock
{
    void operator()() const { return call(); }
    MAKE_CONST_MOCK0(call, void());
};

SCENARIO("Simple actions.")
{
    GIVEN("A write stream.")
    {
        write_mock  writer;
        action_mock closure;
        auto        s = action(writer, closure);

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            auto sender = s.write(2);
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called on the sender.")
            {
                ALLOW_CALL(writer.sender_, submit());
                sender.submit();
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                ALLOW_CALL(writer.sender_, submit(ANY(write_token)))
                    .SIDE_EFFECT(_1(0););
                write_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_)).WITH(_1 == 0);

                sender.submit(callback_mock);
            }
        }

        WHEN("Range write is called.")
        {
            REQUIRE_CALL(writer, write_(vector{2, 3}));
            auto sender = s.write(array{2, 3});
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called.")
            {
                ALLOW_CALL(writer.range_sender_, submit());
                sender.submit();
            }

            WHEN("Asynchronous submit is called.")
            {
                ALLOW_CALL(writer.range_sender_, submit(ANY(completion_token)))
                    .SIDE_EFFECT(_1(0, 2));
                range_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_, _))
                    .WITH(_1 == 0 && _2 == 2);

                sender.submit(callback_mock);
            }
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            REQUIRE_CALL(writer, write_(vector{0, 1, 2}));
            auto sender = s.write(ranges::view::iota(0, 3));
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called.")
            {
                ALLOW_CALL(writer.range_sender_, submit());
                sender.submit();
            }

            WHEN("Asynchronous submit is called.")
            {
                ALLOW_CALL(writer.range_sender_, submit(ANY(completion_token)))
                    .SIDE_EFFECT(_1(0, 3));
                range_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_, _))
                    .WITH(_1 == 0 && _2 == 3);

                sender.submit(callback_mock);
            }
        }
    }

    GIVEN("A read stream.")
    {
        read_mock   reader;
        action_mock closure;
        auto        s = action(reader, closure);

        WHEN("A single value is read.")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
            auto sender = s.read();
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called on the sender")
            {
                ALLOW_CALL(reader.sender_, submit()).RETURN(1);
                auto v = sender.submit();
                REQUIRE(v == 1);
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                ALLOW_CALL(reader.sender_, submit(ANY(read_token<char>)))
                    .SIDE_EFFECT(_1(0, 1););
                read_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_, _))
                    .WITH(_1 == 0 && _2 == 1);

                sender.submit(callback_mock);
            }
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{2, 3});
            std::array<int, 2> a;
            auto               sender = s.read(a);
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called on the sender")
            {
                ALLOW_CALL(reader.range_sender_, submit());
                sender.submit();
                REQUIRE_THAT(a, Equals(array{2, 3}));
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                ALLOW_CALL(reader.range_sender_, submit(ANY(completion_token)))
                    .SIDE_EFFECT(_1(0, 2););
                read_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_, _))
                    .WITH(_1 == 0 && _2 == 2);
                sender.submit(callback_mock);

                REQUIRE_THAT(a, Equals(array{2, 3}));
            }
        }
    }
}

SCENARIO("R-value writer and callback")
{
    [[maybe_unused]] auto s = action(move_only{}, [] {});
}

SCENARIO("Pipe operator")
{
    write_mock writer;

    [[maybe_unused]] auto s = writer | action([] {});
}
