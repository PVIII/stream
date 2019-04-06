#ifndef STREAM_TRANSFORM_H_
#define STREAM_TRANSFORM_H_

#include "callback.h"

#include "delegate/Delegate.h"

namespace stream
{
template<class Stream, class F> class transform
{
    Stream& stream_;
    F       func_;

  public:
    transform(Stream& stream, F&& f) : stream_(stream), func_(f) {}

    void write(auto const& v) { stream_.write(func_(v)); }

    void write(auto const& v, completion_token&& c)
    {
        stream_.write(func_(v), c);
    }
};
} // namespace stream

#endif // STREAM_TRANSFORM_H_
