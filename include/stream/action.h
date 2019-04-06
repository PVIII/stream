#ifndef STREAM_ACTION_H_
#define STREAM_ACTION_H_

#include "callback.h"

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

    void write(auto const& v, completion_token&& c)
    {
        pre_();
        stream_.write(v, c);
    }
};
} // namespace stream

#endif // STREAM_ACTION_H_
