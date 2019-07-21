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
  public:
    template<class C> struct context
    {
        C                    child_;
        action<Stream, Pre>& stream_;

        context(C&& c, action<Stream, Pre>& s) : child_(c), stream_(s) {}

        template<class T> void submit(T&& token)
        {
            stream_.pre_();
            child_.submit(std::forward<T>(token));
        }

        void submit()
        {
            stream_.pre_();
            child_.submit();
        }
    };

  private:
    Stream& stream_;
    Pre     pre_;

  public:
    action(Stream& stream, Pre&& pre) : stream_(stream), pre_(pre) {}

    template<ranges::Range R> auto write(R&& r)
    {
        return context(stream_.write(std::forward<R>(r)), *this);
    }

    template<class V> auto write(V&& v)
    {
        return context{stream_.write(std::forward<V>(v)), *this};
    }
};
} // namespace stream

#endif // LIBSTREAM_ACTION_HPP_
