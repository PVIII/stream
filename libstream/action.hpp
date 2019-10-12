/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_ACTION_HPP_
#define LIBSTREAM_ACTION_HPP_

#include <libstream/callback.hpp>
#include <libstream/concepts/executor.hpp>
#include <libstream/concepts/pipe.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
namespace detail
{
template<class Token, class Pre, class Child, class Stream> struct context
{
    Pre           pre_;
    Child         child_;
    const Stream& stream_;
    Token         token_;
    bool          child_submitted_;

    context(Pre&& p, Child&& c, const Stream& s)
        : pre_(std::forward<Pre>(p)), child_(std::forward<Child>(c)), stream_(s)
    {
    }

    void pre_handler()
    {
        child_submitted_ = true;
        child_.submit(std::move(token_));
    }

    template<class T> void submit(T&& t)
    {
        token_           = t;
        child_submitted_ = false;

        using this_t = context<Token, Pre, Child, Stream>;
        pre_.submit(token<>{
            t.error, t.cancelled,
            done_token::template create<this_t, &this_t::pre_handler>(this)});
    }

    auto submit()
    {
        pre_.submit();
        return child_.submit();
    }

    void cancel()
    {
        if(child_submitted_) { child_.cancel(); }
        else
        {
            pre_.cancel();
        }
    }
};

template<class T, class P, class C, class S>
context<T, P, C, S> make_context(P&& p, C&& c, S&& s)
{
    return context<T, P, C, S>{std::forward<P>(p), std::forward<C>(c),
                               std::forward<S>(s)};
}

} // namespace detail

template<Streamable S, class Pre> requires Executable<Pre, void> class action_fn
{
    S   stream_;
    Pre pre_;

  public:
    action_fn(S&& stream, Pre&& pre)
        : stream_(std::forward<S>(stream)), pre_(pre)
    {
    }

    auto read() const requires PureReadStreamable<S>
    {
        using child_token = read_token<decltype(stream_.read().submit())>;
        return detail::make_context<child_token>(pre_(), stream_.read(), *this);
    }

    template<ranges::Range R>
    auto read(R&& r) const requires PureReadStreamable<S>
    {
        return detail::make_context<completion_token>(
            pre_(), stream_.read(std::forward<R>(r)), *this);
    }

    template<ranges::InputRange R>
    auto write(R&& r) const requires PureWriteStreamable<S>
    {
        return detail::make_context<completion_token>(
            pre_(), stream_.write(std::forward<R>(r)), *this);
    }

    template<class V> auto write(V&& v) const requires PureWriteStreamable<S>
    {
        return detail::make_context<write_token>(
            pre_(), stream_.write(std::forward<V>(v)), *this);
    }

    template<class V>
    auto readwrite(V&& v) const requires ReadWriteStreamable<S>
    {
        using child_token = read_token<decltype(stream_.readwrite(v).submit())>;
        return detail::make_context<child_token>(
            pre_(), stream_.readwrite(std::forward<V>(v)), *this);
    }

    template<ranges::InputRange Rin, ranges::Range Rout>
    auto readwrite(Rin&& rin, Rout&& rout) const requires ReadWriteStreamable<S>
    {
        return detail::make_context<completion_token>(
            pre_(),
            stream_.readwrite(std::forward<Rin>(rin), std::forward<Rout>(rout)),
            *this);
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
