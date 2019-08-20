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

        context(C&& c, action<Stream, Pre>& s)
            : child_(std::forward<C>(c)), stream_(s)
        {
        }

        template<class T> void submit(T&& token)
        {
            stream_.pre_();
            child_.submit(std::forward<T>(token));
        }

        auto submit()
        {
            stream_.pre_();
            return child_.submit();
        }
    };

  private:
    Stream& stream_;
    Pre     pre_;

  public:
    action(Stream& stream, Pre&& pre) : stream_(stream), pre_(pre) {}

    auto read()
    {
        return context<decltype(stream_.read())>{stream_.read(), *this};
    }

    template<ranges::Range R> auto read(R&& r)
    {
        return context<decltype(stream_.read(std::forward<R>(r)))>{
            stream_.read(std::forward<R>(r)), *this};
    }

    template<ranges::Range R> auto write(R&& r)
    {
        return context<decltype(stream_.write(std::forward<R>(r)))>{
            stream_.write(std::forward<R>(r)), *this};
    }

    template<class V> auto write(V&& v)
    {
        return context<decltype(stream_.write(std::forward<V>(v)))>{
            stream_.write(std::forward<V>(v)), *this};
    }
};

template<class Stream, class Pre>
action(Stream& stream, Pre&& pre)->action<Stream, Pre>;

} // namespace stream

#endif // LIBSTREAM_ACTION_HPP_
