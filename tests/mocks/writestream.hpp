/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_WRITESTREAM_HPP_
#define TESTS_MOCKS_WRITESTREAM_HPP_

#include <libstream/callback.hpp>

#include <cassert>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/range>
#include <vector>

namespace ranges = std::experimental::ranges;

namespace stream
{
struct write_interface
{
    struct submit_interface
    {
        virtual void submit()                = 0;
        virtual void submit(write_token&& t) = 0;
    };

    virtual submit_interface& write(char) = 0;
};

struct write_stream
{
    using value_type = char;

    value_type              v_ = 0x7F;
    std::vector<value_type> vs_;

    write_token      callback_;
    completion_token range_callback_;
    std::size_t      n_;

    template<ranges::Range R> struct range_context
    {
        R             range_;
        write_stream& stream_;
        bool          submitted_ = false;

        void submit(completion_token&& t)
        {
            assert(!submitted_);

            submitted_              = true;
            stream_.range_callback_ = t;

            stream_.vs_.clear();
            ranges::copy(range_, ranges::back_inserter(stream_.vs_));
            stream_.n_ = stream_.vs_.size();
        }

        void submit() { submit({}); }
    };

    struct write_context
    {
        value_type    value_;
        write_stream& stream_;

        void submit(write_token&& t)
        {
            stream_.callback_ = t;
            stream_.v_        = value_;
        }

        void submit() { submit({}); }
    };

    auto write(value_type v) { return write_context{v, *this}; }

    template<ranges::Range R> auto write(R&& r)
    {
        return range_context<R>{std::forward<R>(r), *this};
    }

    void callback() const { callback_(0); }

    void range_callback() const { range_callback_(0, n_); }
};

} // namespace stream

#endif // TESTS_MOCKS_WRITESTREAM_HPP_
