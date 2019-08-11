/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include "action.hpp"

#include <tests/helpers/range_matcher.hpp>
#include <tests/mocks/callback.hpp>
#include <tests/mocks/readstream.hpp>
#include <tests/mocks/writestream.hpp>

#include <catch2/catch.hpp>
#include <fakeit.hpp>

#include <array>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>

using namespace stream;
using namespace std;
using namespace fakeit;
namespace ranges = std::experimental::ranges;

SCENARIO("Simple actions.")
{
    struct action_interface
    {
        virtual void operator()() = 0;
    };
    Mock<action_interface> action_mock;
    Fake(Method(action_mock, operator()));

    GIVEN("A write stream.")
    {
        Mock<write_interface::submit_interface> sender_mock;
        Mock<write_interface>                   stream_mock;
        When(Method(stream_mock, write)).Return(sender_mock.get());

        auto s = action(stream_mock.get(), action_mock.get());

        WHEN("Write is called.")
        {
            auto sender = s.write(2);
            Verify(Method(stream_mock, write).Using(2)).Once();

            WHEN("Synchronous submit is called on the sender.")
            {
                Fake(OverloadedMethod(sender_mock, submit, void()));
                sender.submit();

                Verify(Method(action_mock, operator()) +
                       OverloadedMethod(sender_mock, submit, void()))
                    .Once();
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                When(
                    OverloadedMethod(sender_mock, submit, void(write_token &&)))
                    .Do([](auto&& t) { t(0); });
                Mock<write_callback_interface> callback_mock;
                Fake(Method(callback_mock, operator()));

                sender.submit(callback_mock.get());

                Verify(Method(action_mock, operator()) +
                       OverloadedMethod(sender_mock, submit,
                                        void(write_token &&)) +
                       Method(callback_mock, operator()).Using(0))
                    .Once();
                VerifyNoOtherInvocations(callback_mock);
            }
        }

        VerifyNoOtherInvocations(stream_mock, stream_mock);
        VerifyNoOtherInvocations(sender_mock, stream_mock);
    }

    GIVEN("A read stream.")
    {
        Mock<read_interface::submit_interface> sender_mock;
        Mock<read_interface>                   stream_mock;
        When(Method(stream_mock, read)).Return(sender_mock.get());

        auto s = action(stream_mock.get(), action_mock.get());

        WHEN("A single value is read.")
        {
            auto sender = s.read();
            Verify(Method(stream_mock, read)).Once();

            WHEN("Synchronous submit is called on the sender")
            {
                When(OverloadedMethod(sender_mock, submit, char())).Return(1);
                auto v = sender.submit();

                REQUIRE(v == 1);
                Verify(Method(action_mock, operator()) +
                       OverloadedMethod(sender_mock, submit, char()))
                    .Once();
            }

            WHEN("Asynchronous submit is called on the sender.")
            {
                Mock<read_callback_interface> callback_mock;
                Fake(Method(callback_mock, operator()));
                When(OverloadedMethod(sender_mock, submit,
                                      void(read_token<char> &&)))
                    .Do([](auto&& t) { t(0, 1); });
                sender.submit(callback_mock.get());

                Verify(Method(stream_mock, read) +
                       Method(action_mock, operator()) +
                       OverloadedMethod(sender_mock, submit,
                                        void(read_token<char> &&)) +
                       Method(callback_mock, operator()).Using(0, 1))
                    .Once();
                VerifyNoOtherInvocations(callback_mock);
            }
        }

        VerifyNoOtherInvocations(sender_mock);
        VerifyNoOtherInvocations(stream_mock);
    }

    VerifyNoOtherInvocations(action_mock);
}

SCENARIO("Actions with ranges.")
{
    GIVEN("A write stream that increments a variable for each write.")
    {
        write_stream ws;
        char         counter = 0;
        auto         s       = action(ws, [&]() { ++counter; });

        WHEN("[2, 3] is written.")
        {
            s.write(array{2, 3}).submit();
            THEN("[2, 3] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{2, 3}));
            }
            THEN("The counter is incremented by one.")
            {
                REQUIRE(counter == 1);
            }
        }

        WHEN("[3, 4] is written asynchronously.")
        {
            auto callback = [&](auto ec, auto n) {
                THEN("[3, 4] is written.")
                {
                    REQUIRE(ranges::equal(ws.vs_, array{3, 4}));
                }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The number of written elements is 2.")
                {
                    REQUIRE(n == 2);
                }
                THEN("The counter is incremented by one.")
                {
                    REQUIRE(counter == 1);
                }
            };
            auto sender = s.write(array{3, 4});
            sender.submit(callback);
        }

        WHEN("[0, 1, 2] is generated and written.")
        {
            s.write(ranges::view::iota(0, 3)).submit();
            THEN("[0, 1, 2] is written.")
            {
                REQUIRE(ranges::equal(ws.vs_, array{0, 1, 2}));
            }
            THEN("The counter is incremented by exactly one.")
            {
                REQUIRE(counter == 1);
            }
        }
    }
    GIVEN("A read stream that increments a variable for each read.")
    {
        read_stream rs;
        char        counter = 0;
        auto        s       = action(rs, [&]() { ++counter; });

        WHEN("[2, 3] is read.")
        {
            rs.vs_ = {2, 3};
            std::array<int, 2> a;
            s.read(a).submit();
            THEN("The stream reads [2, 3].")
            {
                REQUIRE_THAT(a, Equals(array{2, 3}));
            }
            THEN("The counter is incremented by one.")
            {
                REQUIRE(counter == 1);
            }
        }

        WHEN("[3, 4] is read asynchronously.")
        {
            rs.vs_ = {3, 4};
            array<int, 2> a{0, 0};
            auto          callback = [&](auto ec, auto n) {
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("Two values have been read.") { REQUIRE(n == 2); }
                THEN("The stream reads [3, 4].")
                {
                    REQUIRE_THAT(a, Equals(array{3, 4}));
                }
                THEN("The counter is incremented by one.")
                {
                    REQUIRE(counter == 1);
                }
            };
            auto sender = s.read(a);
            sender.submit(callback);
        }
    }
}
