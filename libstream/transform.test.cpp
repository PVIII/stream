/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/transform.hpp>

#include <tests/helpers/constrained_types.hpp>
#include <tests/helpers/range_matcher.hpp>
#include <tests/helpers/submit_tester.hpp>
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
        auto s = stream::transform_write(writer, [](auto v) { return v + 1; });

        WHEN("Single write is called")
        {
            REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
            auto sender = s.write(1);

            test_sync_submit(writer.sender_, sender);
            test_async_write_submit(writer.sender_, sender);
            test_async_write_submit(writer.sender_, sender, 1);
        }

        WHEN("Range write is called")
        {
            REQUIRE_CALL(writer, write_(vector{2, 3}));
            auto a      = array{1, 2};
            auto sender = s.write(a);

            test_sync_submit(writer.range_sender_, sender);
            test_async_range_submit(writer.range_sender_, sender, 2, 2);
            test_async_range_submit(writer.range_sender_, sender, 2, 2, 1);
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            REQUIRE_CALL(writer, write_(vector{1, 2, 3}));
            auto sender = s.write(ranges::view::iota(0, 3));

            test_sync_submit(writer.range_sender_, sender);
            test_async_range_submit(writer.range_sender_, sender, 3, 3);
            test_async_range_submit(writer.range_sender_, sender, 3, 3, 1);
        }
    }

    GIVEN("A read stream that adds one.")
    {
        read_mock reader;
        auto s = stream::transform_read(reader, [](auto v) { return v + 1; });

        WHEN("Single read is called")
        {
            REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
            auto sender = s.read();

            test_sync_read_submit(reader.sender_, sender, 1, 2);
            test_async_read_submit(reader.sender_, sender, 1, 2);
            test_async_read_submit(reader.sender_, sender, 1, 2, 1);
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{1, 2});
            std::array<int, 2> a;
            auto               sender = s.read(a);
            REQUIRE_THAT(a, Equals(array{2, 3}));

            test_sync_submit(reader.range_sender_, sender);
            test_async_range_submit(reader.range_sender_, sender, 2, 2);
            test_async_range_submit(reader.range_sender_, sender, 2, 2, 1);
        }
    }
}

SCENARIO("Change the value type.")
{
    GIVEN("A write stream that adds a pair.")
    {
        write_mock writer;
        auto       s = stream::transform_write(
            writer, [](auto v) { return v.first + v.second; });

        WHEN("Single write is called")
        {
            REQUIRE_CALL(writer, write(3)).LR_RETURN(writer.sender_);
            [[maybe_unused]] auto sender = s.write(pair{1, 2});
        }
    }

    GIVEN("A read stream that converts an integer to a complex number.")
    {
        read_mock reader;
        auto      s = stream::transform_read(reader, [](auto v) {
            return std::complex{v, 3};
        });
        REQUIRE_CALL(reader, read()).LR_RETURN(reader.sender_);
        auto sender = s.read();

        test_sync_read_submit(reader.sender_, sender, 2, std::complex{2, 3});
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
                stream::transform_write(writer, [](auto v) { return v + 1; });
        }
    }

    GIVEN("Reader")
    {
        read_mock reader;
        THEN("A constant adaptor can refer to it.")
        {
            [[maybe_unused]] const auto s =
                stream::transform_read(reader, [](auto v) { return v + 1; });
        }
    }
}

SCENARIO("R-value writer and callback")
{
    [[maybe_unused]] auto s =
        stream::transform_read(move_only_reader{}, [](int v) { return v; });
}

SCENARIO("Pipe operator")
{
    write_mock writer;

    [[maybe_unused]] auto s =
        writer | stream::transform_write([](int v) { return v; });
}

SCENARIO("Double transform")
{
    GIVEN("A write stream.")
    {
        write_mock writer;

        auto s = stream::transform_write(
            stream::transform_write(writer, [](auto v) { return v + 1; }),
            [](auto v) { return v * 2; });

        WHEN("Single value write is called.")
        {
            REQUIRE_CALL(writer, write(5)).LR_RETURN(writer.sender_);
            auto sender = s.write(2);

            test_sync_submit(writer.sender_, sender);
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
            auto s =
                stream::transform_write(writer, [](auto v) { return v + 1; });

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
