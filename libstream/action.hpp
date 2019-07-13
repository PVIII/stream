/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_ACTION_HPP_
#define LIBSTREAM_ACTION_HPP_

#include <libstream/callback.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<class Stream, class Pre> class action
{
    Stream& stream_;
    Pre     pre_;

  public:
    action(Stream& stream, Pre&& pre) : stream_(stream), pre_(pre) {}

    void write(auto const& v)
    {
        pre_();
        stream_.write(v);
    }

    void write(ranges::Range const& r, completion_token&& t)
    {
        pre_();
        stream_.write(r, std::forward<completion_token>(t));
    }

    void write(auto const& v, write_token&& c)
    {
        pre_();
        stream_.write(v, c);
    }
};
} // namespace stream

#endif // LIBSTREAM_ACTION_HPP_
