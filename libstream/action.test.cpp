/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#include "action.hpp"

#include <tests/helpers/range_matcher.hpp>
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

SCENARIO("Actions with single values.")
{
    GIVEN("A write stream that executes an action before every write.")
    {
        struct action_interface
        {
            virtual void operator()() = 0;
        };
        Mock<submit_interface> sender_mock;
        Fake(OverloadedMethod(sender_mock, submit, void()));
        When(OverloadedMethod(sender_mock, submit, void(write_token &&)))
            .Do([](auto&& t) { t(0); });
        Mock<write_interface> stream_mock;
        When(Method(stream_mock, write)).Return(sender_mock.get());
        Mock<action_interface> action_mock;
        Fake(Method(action_mock, operator()));

        auto s = action(stream_mock.get(), action_mock.get());

        WHEN("A single value is written.")
        {
            s.write(2).submit();

            THEN("write, prefix and submit are called in this order.")
            {
                Verify(Method(stream_mock, write).Using(2) +
                       Method(action_mock, operator()) +
                       OverloadedMethod(sender_mock, submit, void()))
                    .Once();
                VerifyNoOtherInvocations(stream_mock);
                VerifyNoOtherInvocations(action_mock);
                VerifyNoOtherInvocations(sender_mock);
            }
        }
        WHEN("A single value is written asynchronously.")
        {
            struct write_interface
            {
                virtual void operator()(stream::error_code) = 0;
            };
            Mock<write_interface> callback_mock;
            Fake(Method(callback_mock, operator()));

            s.write(3).submit(callback_mock.get());

            Verify(Method(stream_mock, write).Using(3) +
                   Method(action_mock, operator()) +
                   OverloadedMethod(sender_mock, submit, void(write_token &&)) +
                   Method(callback_mock, operator()).Using(0))
                .Once();
            VerifyNoOtherInvocations(stream_mock);
            VerifyNoOtherInvocations(action_mock);
            VerifyNoOtherInvocations(sender_mock);
            VerifyNoOtherInvocations(callback_mock);
        }
    }

    GIVEN("A read stream that increments a variable for each read.")
    {
        read_stream rs;
        char        counter = 0;
        auto        s       = action(rs, [&]() { ++counter; });

        WHEN("A single value is read.")
        {
            rs.v_  = 1;
            auto v = s.read().submit();
            THEN("The value is written.") { REQUIRE(v == 1); }
            THEN("The counter is incremented by one.")
            {
                REQUIRE(counter == 1);
            }
        }

        WHEN("A single value is read asynchronously.")
        {
            rs.v_         = 1;
            auto callback = [&](auto ec, auto v) {
                THEN("The value is written.") { REQUIRE(v == 1); }
                THEN("No error is returned.") { REQUIRE(ec == 0); }
                THEN("The counter is incremented by one.")
                {
                    REQUIRE(counter == 1);
                }
            };
            auto sender = s.read();
            THEN("Before submit the counter is not incremented.")
            {
                REQUIRE(counter == 0);
            }
            sender.submit(callback);
        }
    }
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
