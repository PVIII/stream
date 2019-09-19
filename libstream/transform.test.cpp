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
#include <tests/mocks/readwritestream.hpp>
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
            test_async_range_submit(writer.range_sender_, sender, {2, 2});
            test_async_range_submit(writer.range_sender_, sender, {2, 2},
                                    dummy_error);
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            REQUIRE_CALL(writer, write_(vector{1, 2, 3}));
            auto sender = s.write(ranges::view::iota(0, 3));

            test_sync_submit(writer.range_sender_, sender);
            test_async_range_submit(writer.range_sender_, sender, {3, 3});
            test_async_range_submit(writer.range_sender_, sender, {3, 3},
                                    dummy_error);
        }

        WHEN("Writing less than buffer size.")
        {
            writer.restrict_to_ = 1;

            REQUIRE_CALL(writer, write_(vector{2}));
            auto a      = array{1, 2};
            auto sender = s.write(a);

            test_sync_submit(writer.range_sender_, sender);
            test_async_range_submit(writer.range_sender_, sender, {1, 1});
            test_async_range_submit(writer.range_sender_, sender, {1, 1},
                                    dummy_error);
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

            test_sync_read_submit(reader.sender_, sender, test_pair{1, 2});
            test_async_read_submit(reader.sender_, sender, test_pair{1, 2});
            test_async_read_submit(reader.sender_, sender, test_pair{1, 2},
                                   dummy_error);
        }

        WHEN("A range is read.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{1, 2});
            std::array<int, 2> a;
            auto               sender = s.read(a);
            REQUIRE_THAT(a, Equals(array{2, 3}));

            test_sync_submit(reader.range_sender_, sender);
            test_async_range_submit(reader.range_sender_, sender,
                                    test_pair{2, 2});
            test_async_range_submit(reader.range_sender_, sender,
                                    test_pair{2, 2}, dummy_error);
        }

        WHEN("Reading less than the buffer size.")
        {
            REQUIRE_CALL(reader, read_(_)).SIDE_EFFECT(_1 = vector{1});
            std::array a{0, 0};
            auto       sender = s.read(a);
            REQUIRE_THAT(a, Equals(std::array{2, 0}));

            test_sync_submit(reader.range_sender_, sender);
            test_async_range_submit(reader.range_sender_, sender,
                                    test_pair{1, 1});
            test_async_range_submit(reader.range_sender_, sender,
                                    test_pair{1, 1}, dummy_error);
        }
    }

    GIVEN("A read-write stream.")
    {
        read_write_mock readwriter;

        GIVEN("That adds one to reads.")
        {
            auto s = stream::transform_read(readwriter,
                                            [](auto v) { return v + 1; });

            WHEN("A single value is read and written.")
            {
                REQUIRE_CALL(readwriter, readwrite(1))
                    .LR_RETURN(readwriter.sender_);
                auto sender = s.readwrite(1);

                WHEN("The operation is not cancelled.")
                {
                    test_sync_read_submit(readwriter.sender_, sender,
                                          test_pair{1, 2});
                    test_async_read_submit(readwriter.sender_, sender,
                                           test_pair{1, 2});
                    test_async_read_submit(readwriter.sender_, sender,
                                           test_pair{1, 2}, dummy_error);
                }
            }

            WHEN("Ranges are read and written.")
            {
                REQUIRE_CALL(readwriter, readwrite_(vector{2, 3}, _))
                    .SIDE_EFFECT(_2 = vector{4, 5});
                std::array<int, 2> a_read;
                std::array         a_write{2, 3};
                auto               sender = s.readwrite(a_write, a_read);
                REQUIRE_THAT(a_read, Equals(array{5, 6}));

                WHEN("The operation is not cancelled.")
                {
                    test_sync_submit(readwriter.range_sender_, sender);
                    test_async_range_submit(readwriter.range_sender_, sender,
                                            {2, 2});
                    test_async_range_submit(readwriter.range_sender_, sender,
                                            {2, 2}, dummy_error);
                }
            }

            WHEN("Buffer sizes are different.")
            {
                REQUIRE_CALL(readwriter, readwrite_(vector{2, 3}, _))
                    .SIDE_EFFECT(_2 = vector{4, 5, 6});
                array a_read{0, 0, 0, 0};
                array a_write{2, 3};
                auto  sender = s.readwrite(a_write, a_read);
                REQUIRE_THAT(a_read, Equals(array{5, 6, 7, 0}));

                WHEN("The operation is not cancelled.")
                {
                    test_sync_submit(readwriter.range_sender_, sender);
                    test_async_range_submit(readwriter.range_sender_, sender,
                                            {3, 3});
                    test_async_range_submit(readwriter.range_sender_, sender,
                                            {3, 3}, dummy_error);
                }
            }
        }

        GIVEN("That adds one to writes.")
        {
            auto s = stream::transform_write(readwriter,
                                             [](auto v) { return v + 1; });

            WHEN("A single value is read and written.")
            {
                REQUIRE_CALL(readwriter, readwrite(2))
                    .LR_RETURN(readwriter.sender_);
                auto sender = s.readwrite(1);

                WHEN("The operation is not cancelled.")
                {
                    test_sync_read_submit(readwriter.sender_, sender,
                                          test_pair{1, 1});
                    test_async_read_submit(readwriter.sender_, sender,
                                           test_pair{1, 1});
                    test_async_read_submit(readwriter.sender_, sender,
                                           test_pair{1, 1}, dummy_error);
                }
            }

            WHEN("Ranges are read and written.")
            {
                REQUIRE_CALL(readwriter, readwrite_(vector{3, 4}, _))
                    .SIDE_EFFECT(_2 = vector{4, 5});
                std::array<int, 2> a_read;
                std::array         a_write{2, 3};
                auto               sender = s.readwrite(a_write, a_read);
                REQUIRE_THAT(a_read, Equals(array{4, 5}));

                WHEN("The operation is not cancelled.")
                {
                    test_sync_submit(readwriter.range_sender_, sender);
                    test_async_range_submit(readwriter.range_sender_, sender,
                                            {2, 2});
                    test_async_range_submit(readwriter.range_sender_, sender,
                                            {2, 2}, dummy_error);
                }
            }
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

        test_sync_read_submit(reader.sender_, sender,
                              test_pair{2, std::complex{2, 3}});
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
