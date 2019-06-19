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
    Stream& stream_;
    F       func_;

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

    auto read(auto& r) const
    {
        auto tr = output_view::transform(r, func_);
        return stream_.read(tr);
    }
};
} // namespace stream

#endif // LIBSTREAM_TRANSFORM_HPP_
