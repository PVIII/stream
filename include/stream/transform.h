#ifndef STREAM_TRANSFORM_H_
#define STREAM_TRANSFORM_H_

#include "callback.h"
#include "output_view/transform.h"
#include <experimental/ranges/range>

namespace stream
{
template<class Stream, class F> class transform
{
    Stream& stream_;
    F       func_;

  public:
    transform(Stream& stream, F&& f) : stream_(stream), func_(f) {}

    void write(auto const& v) { stream_.write(func_(v)); }

    void write(std::experimental::ranges::Range const& r)
    {
        stream_.write(view::transform(r, func_));
    }

    void write(auto const& v, completion_token&& c)
    {
        stream_.write(func_(v), c);
    }

    void write(std::experimental::ranges::Range const& r, completion_token&& c)
    {
        stream_.write(view::transform(r, func_), c);
    }

    auto read() const { return func_(stream_.read()); }

    auto read(auto& r) const
    {
        auto tr = output_view::transform(r, func_);
        return stream_.read(tr);
    }
};
} // namespace stream

#endif // STREAM_TRANSFORM_H_
