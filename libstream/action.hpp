/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_ACTION_HPP_
#define LIBSTREAM_ACTION_HPP_

#include <libstream/callback.hpp>
#include <libstream/pipe.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<class Stream, class Pre> class action_fn
{
  public:
    template<class C> struct context
    {
        C                       child_;
        action_fn<Stream, Pre>& stream_;

        context(C&& c, action_fn<Stream, Pre>& s)
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
    Stream stream_;
    Pre    pre_;

  public:
    action_fn(Stream&& stream, Pre&& pre)
        : stream_(std::forward<Stream>(stream)), pre_(pre)
    {
    }

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
action_fn(Stream&, Pre &&)->action_fn<Stream&, Pre>;
template<class Stream, class Pre>
action_fn(Stream&&, Pre &&)->action_fn<Stream, Pre>;

template<class Pre> class action_pipe
{
    Pre pre_;

  public:
    action_pipe(Pre&& pre) : pre_(pre) {}

    template<class Stream> auto pipe(Stream&& s) const
    {
        return action_fn{std::forward<Stream>(s), pre_};
    }
};

template<class Pre> auto action(Pre&& pre)
{
    return action_pipe{std::forward<Pre>(pre)};
}

template<class Stream, class Pre> auto action(Stream&& stream, Pre&& pre)
{
    return action_fn{std::forward<Stream>(stream), std::forward<Pre>(pre)};
}

} // namespace stream

#endif // LIBSTREAM_ACTION_HPP_
