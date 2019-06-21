/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_TRANSFORM_HPP_
#define LIBSTREAM_TRANSFORM_HPP_

#include <liboutput_view/transform.hpp>
#include <libstream/callback.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<class Stream, class F> class transform
{
    using value_type = typename Stream::value_type;

    Stream& stream_;
    F       func_;

    read_token<value_type> read_callback_;

    void read_handler(error_code ec, value_type v)
    {
        read_callback_(ec, func_(v));
    }

  public:
    transform(Stream& stream, F&& f) : stream_(stream), func_(f) {}

    void write(auto const& v) { stream_.write(func_(v)); }

    void write(ranges::Range const& r)
    {
        stream_.write(ranges::view::transform(r, func_));
    }

    void write(auto const& v, completion_token&& c)
    {
        stream_.write(func_(v), c);
    }

    void write(ranges::Range const& r, completion_token&& c)
    {
        stream_.write(ranges::view::transform(r, func_), c);
    }

    auto read() const { return func_(stream_.read()); }

    auto read(ranges::Range& r)
    {
        auto tr = output_view::transform(r, func_);
        return stream_.read(tr);
    }

    void read(read_token<value_type>&& t)
    {
        read_callback_ = t;

        using this_t = transform<Stream, F>;
        stream_.read(read_token<value_type>::template create<
                     this_t, &this_t::read_handler>(this));
    }
};
} // namespace stream

#endif // LIBSTREAM_TRANSFORM_HPP_
