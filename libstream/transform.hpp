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
#include <libstream/pipe.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<Stream S, class F> class transform_fn
{
  public:
    template<class C> struct range_context
    {
        C child_;

        range_context(C&& c) : child_(c) {}

        void submit(completion_token&& t)
        {
            child_.submit(std::forward<completion_token>(t));
        }

        auto submit() { return child_.submit(); }
    };
    template<class C> struct read_context
    {
        using value_type = decltype(std::declval<C>().submit());

        C                      child_;
        read_token<value_type> token_;
        transform_fn<S, F>&    stream_;

        read_context(C&& c, transform_fn<S, F>& s) : child_(c), stream_(s) {}

        void handler(error_code ec, value_type v)
        {
            token_(ec, stream_.func_(v));
        }

        auto submit() { return stream_.func_(child_.submit()); }

        void submit(read_token<value_type>&& t)
        {
            token_       = t;
            using this_t = read_context<C>;
            child_.submit(read_token<value_type>::template create<
                          this_t, &this_t::handler>(this));
        }
    };
    template<class C> struct write_context
    {
        C child_context_;

        write_context(C&& c) : child_context_(c) {}

        void submit(write_token&& t)
        {
            child_context_.submit(std::forward<write_token>(t));
        }

        void submit() { child_context_.submit(); }
    };

  private:
    S stream_;
    F func_;

    template<class C> auto make_read_context(C&& c, transform_fn<S, F>& s)
    {
        return read_context<C>{std::forward<C>(c), s};
    }

  public:
    transform_fn(S&& stream, F&& f)
        : stream_(std::forward<S>(stream)), func_(std::forward<F>(f))
    {
    }

    template<class V> auto write(V&& v)
    {
        return write_context<decltype(stream_.write(std::forward<V>(
            func_(v))))>{stream_.write(std::forward<V>(func_(v)))};
    }

    template<ranges::Range R> auto write(R&& r)
    {
        return range_context<decltype(
            stream_.write(ranges::view::transform(std::forward<R>(r), func_)))>{
            stream_.write(ranges::view::transform(std::forward<R>(r), func_))};
    }

    auto read() requires ReadStream<S>
    {
        return make_read_context(stream_.read(), *this);
    }

    auto read(ranges::Range&& r)
    {
        return range_context<decltype(stream_.read(output_view::transform(
            r, func_)))>{stream_.read(output_view::transform(r, func_))};
    }
};

template<class S, class F> transform_fn(S&, F &&)->transform_fn<Stream&, F>;
template<class S, class F> transform_fn(S&&, F &&)->transform_fn<Stream, F>;

template<class F> class transform_pipe
{
    F f_;

  public:
    transform_pipe(F&& f) : f_(f) {}

    template<class S> auto pipe(S&& s) const
    {
        return transform_fn<S, F>{std::forward<S>(s), F(f_)};
    }
};

template<class F> auto transform(F&& f)
{
    return transform_pipe{std::forward<F>(f)};
}

template<class S, class F> auto transform(S&& stream, F&& f)
{
    return transform_fn<S, F>{std::forward<S>(stream), std::forward<F>(f)};
}

} // namespace stream

#endif // LIBSTREAM_TRANSFORM_HPP_
