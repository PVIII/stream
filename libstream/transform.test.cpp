/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/transform.hpp>

#include <tests/helpers/constrained_types.hpp>
#include <tests/helpers/range_matcher.hpp>
#include <tests/mocks/callback.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include <array>
#include <complex>
#include <deque>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>
#include <list>
#include <utility>

using namespace stream;
using namespace std;
namespace ranges = std::experimental::ranges;
using trompeloeil::_;

SCENARIO("Transformations with single values.")
{
    GIVEN("A write stream that adds one.")
    {
        write_mock writer;
        auto       s = stream::transform(writer, [](auto v) { return v + 1; });

        WHEN("Single write is called")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            auto sender = s.write(1);

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
            auto sender = s.write(a);

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

        WHEN("[0, 1, 2] is generated and written.")
        {
            REQUIRE_CALL(writer, write_(vector{1, 2, 3}));
            auto sender = s.write(ranges::view::iota(0, 3));

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

    GIVEN("A read stream that adds one.")
    {
        read_mock reader;
        auto      s = stream::transform(reader, [](auto v) { return v + 1; });

        WHEN("Single read is called")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
            auto sender = s.read();

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
            auto               sender = s.read(a);

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

SCENARIO("Change the value type.")
{
    GIVEN("A write stream that adds a pair.")
    {
        write_mock writer;
        auto       s = stream::transform(writer,
                                   [](auto v) { return v.first + v.second; });

        WHEN("Single write is called")
        {
            REQUIRE_CALL(writer, write(3)).LR_RETURN(writer.sender_);
            [[maybe_unused]] auto sender = s.write(pair{1, 2});
        }
    }

    GIVEN("A read stream that converts an integer to a complex number.")
    {
        read_mock reader;
        auto      s = stream::transform(reader, [](auto v) {
            return std::complex{v, 3};
        });
        REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
        auto sender = s.read();

        WHEN("Synchronous submit is called on the sender.")
        {
            REQUIRE_CALL(reader.sender_, submit()).RETURN(2);
            auto v = sender.submit();
            REQUIRE(v == std::complex{2, 3});
        }
    }
}

SCENARIO("Const transform adaptor.")
{
    GIVEN("Writer")
    {
        write_mock writer;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s =
                stream::transform(writer, [](auto v) { return v + 1; });
        }
    }

    GIVEN("Reader")
    {
        read_mock reader;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s =
                stream::transform(reader, [](auto v) { return v + 1; });
        }
    }
}

SCENARIO("R-value writer and callback")
{
    [[maybe_unused]] auto s =
        stream::transform(move_only_reader{}, [](int v) { return v; });
}

SCENARIO("Pipe operator")
{
    write_mock writer;

    [[maybe_unused]] auto s =
        writer | stream::transform([](int v) { return v; });
}

SCENARIO("Double transform")
{
    GIVEN("A write stream.")
    {
        write_mock writer;

        auto s = stream::transform(
            stream::transform(writer, [](auto v) { return v + 1; }),
            [](auto v) { return v * 2; });

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(writer, write(5)).LR_RETURN(writer.sender_);
            REQUIRE_CALL(writer.sender_, submit());
            auto sender = s.write(2);
            sender.submit();
        }
    }
}

SCENARIO("Random access range is preserved.")
{
    GIVEN("A write stream.")
    {
        write_mock writer{true};
        ALLOW_CALL(writer, write_(ANY(vector<int>)));

        WHEN("It is transformed.")
        {
            auto s = stream::transform(writer, [](auto v) { return v + 1; });

            WHEN("A bidirectional range is written.")
            {
                REQUIRE_CALL(writer, bidirectional_write_());
                std::list<int> l;
                s.write(l);
            }

            WHEN("A random-access range is written.")
            {
                REQUIRE_CALL(writer, random_access_write_());
                std::deque<int> v;
                s.write(v);
            }
        }
    }
}
