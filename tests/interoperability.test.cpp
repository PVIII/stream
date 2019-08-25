/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/action.hpp>
#include <libstream/transform.hpp>

#include <tests/helpers/range_matcher.hpp>
#include <tests/mocks/callback.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/writestream.hpp>

using namespace stream;
using namespace std;
namespace ranges = std::experimental::ranges;
using trompeloeil::_;

struct action_mock
{
    void operator()() const { return call(); }
    MAKE_CONST_MOCK0(call, void());
};

SCENARIO("Actions and transformations.")
{
    action_mock closure;

    GIVEN("A write stream.")
    {
        write_mock writer;

        auto s  = stream::transform(writer, [](auto v) { return v + 1; });
        auto s2 = stream::action(s, closure);

        WHEN("Single write is called")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            auto sender = s2.write(1);
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called on the sender.")
            {
                REQUIRE_CALL(writer.sender_, submit());
                sender.submit();
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                REQUIRE_CALL(writer.sender_, submit(ANY(write_token)))
                    .SIDE_EFFECT(_1(0););
                write_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_)).WITH(_1 == 0);

                sender.submit(callback_mock);
            }
        }

        WHEN("Range write is called")
        {
            REQUIRE_CALL(writer, write_(vector{2, 3}));
            auto a      = array{1, 2};
            auto sender = s2.write(a);
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called on the sender.")
            {
                REQUIRE_CALL(writer.range_sender_, submit());
                sender.submit();
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                ALLOW_CALL(writer.range_sender_, submit(ANY(completion_token)))
                    .SIDE_EFFECT(_1(0, 2));
                range_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_, _))
                    .WITH(_1 == 0 && _2 == 2);

                sender.submit(callback_mock);
            }
        }
    }

    GIVEN("A read stream that adds one.")
    {
        read_mock reader;

        auto s  = stream::transform(reader, [](auto v) { return v + 1; });
        auto s2 = stream::action(s, closure);

        WHEN("Single read is called")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
            auto sender = s2.read();
            REQUIRE_CALL(closure, call());

            WHEN("Synchronous submit is called on the sender.")
            {
                REQUIRE_CALL(reader.sender_, submit()).RETURN(1);
                auto v = sender.submit();
                REQUIRE(v == 2);
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                REQUIRE_CALL(reader.sender_, submit(ANY(read_token<int>)))
                    .SIDE_EFFECT(_1(0, 1););
                read_callback_mock callback_mock;
                REQUIRE_CALL(callback_mock, call(_, _))
                    .WITH(_1 == 0 && _2 == 2);

                sender.submit(callback_mock);
            }
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{1, 2});
            std::array<int, 2> a;

            auto sender = s2.read(a);
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

SCENARIO("Pipe interoperability.")
{
    action_mock closure;
    write_mock  writer;

    auto s = writer | stream::transform([](auto v) { return v + 1; }) |
             stream::action(closure);

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
