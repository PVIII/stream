/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include <libstream/pipe.hpp>

#include <libstream/action.hpp>
#include <libstream/transform.hpp>

#include <tests/helpers/submit_tester.hpp>
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

    THEN("A pure pipe can be created.")
    {
        auto p1 = stream::transform_write([](auto v) { return v + 1; }) |
                  action(closure);

        GIVEN("A write stream.")
        {
            write_mock writer;

            THEN("The pure pipe can be applied.")
            {
                auto s = writer | p1;

                WHEN("Single write is called")
                {
                    REQUIRE_CALL(writer, write(2)).LR_RETURN(writer.sender_);
                    auto sender = s.write(1);
                    REQUIRE_CALL(closure, call());

                    test_sync_submit(writer.sender_, sender);
                }
            }
        }
        GIVEN("Another pure pipe.")
        {
            const auto p2 =
                stream::transform_write([](auto v) { return v * 2; }) |
                action(closure);

            THEN("They can be combined.")
            {
                auto p = p1 | p2;

                GIVEN("A write stream.")
                {
                    write_mock writer;

                    THEN("The combined pipe can be applied.")
                    {
                        auto s = writer | p;

                        WHEN("Single write is called")
                        {
                            REQUIRE_CALL(writer, write(3))
                                .LR_RETURN(writer.sender_);
                            auto sender = s.write(1);
                            REQUIRE_CALL(closure, call()).TIMES(2);

                            test_sync_submit(writer.sender_, sender);
                        }
                    }
                }
            }

            GIVEN("A write stream.")
            {
                write_mock writer;

                THEN("They can be applied successively.")
                {
                    auto s = writer | p1 | p2;

                    WHEN("Single write is called")
                    {
                        REQUIRE_CALL(writer, write(3))
                            .LR_RETURN(writer.sender_);
                        auto sender = s.write(1);
                        REQUIRE_CALL(closure, call()).TIMES(2);

                        test_sync_submit(writer.sender_, sender);
                    }
                }
            }
        }
    }

    THEN("A const pure pipe can be created.")
    {
        const auto p = stream::transform_write([](auto v) { return v + 1; }) |
                       action(closure);

        GIVEN("A write stream.")
        {
            write_mock writer;

            THEN("The const pure pipe can be applied.")
            {
                [[maybe_unused]] auto s = writer | p;
            }
        }
    }
}
