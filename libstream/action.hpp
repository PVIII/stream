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

#include <type_traits>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<Streamable S, ranges::RegularInvocable Pre> class action_fn
{
  public:
    using value_type = typename std::remove_reference_t<S>::value_type;

    template<class C> struct context
    {
        C                  child_;
        action_fn<S, Pre>& stream_;

        context(C&& c, action_fn<S, Pre>& s)
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
    S   stream_;
    Pre pre_;

  public:
    action_fn(S&& stream, Pre&& pre)
        : stream_(std::forward<S>(stream)), pre_(pre)
    {
    }

    auto read() requires ReadStreamable<S>
    {
        return context<decltype(stream_.read())>{stream_.read(), *this};
    }

    template<ranges::Range R>
    auto
    read(R&& r) requires ReadStreamable<S>&& ranges::OutputRange<R, value_type>
    {
        return context<decltype(stream_.read(std::forward<R>(r)))>{
            stream_.read(std::forward<R>(r)), *this};
    }

    template<ranges::InputRange R> auto write(R&& r) requires WriteStreamable<S>
    {
        return context<decltype(stream_.write(std::forward<R>(r)))>{
            stream_.write(std::forward<R>(r)), *this};
    }

    template<class V> auto write(V&& v) requires WriteStreamable<S>
    {
        return context<decltype(stream_.write(std::forward<V>(v)))>{
            stream_.write(std::forward<V>(v)), *this};
    }
};

template<Streamable S, ranges::RegularInvocable Pre>
action_fn(S&, Pre &&)->action_fn<S&, Pre>;
template<Streamable S, ranges::RegularInvocable Pre>
action_fn(S&&, Pre &&)->action_fn<S, Pre>;

template<ranges::RegularInvocable Pre> class action_pipe
{
    Pre pre_;

  public:
    action_pipe(Pre&& pre) : pre_(pre) {}

    template<Streamable S> Streamable pipe(S&& s) const
    {
        return action_fn{std::forward<S>(s), pre_};
    }
};

template<ranges::RegularInvocable Pre> Pipeable action(Pre&& pre)
{
    return action_pipe<Pre>{std::forward<Pre>(pre)};
}

template<Streamable S, ranges::RegularInvocable Pre>
Streamable action(S&& stream, Pre&& pre)
{
    return action_fn{std::forward<S>(stream), std::forward<Pre>(pre)};
}

} // namespace stream

#endif // LIBSTREAM_ACTION_HPP_
