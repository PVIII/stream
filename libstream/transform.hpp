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
namespace detail
{
template<class C> struct range_context
{
    C child_;

    range_context(C&& c) : child_(c) {}

    void submit(completion_token&& t)
    {
        child_.submit(std::forward<completion_token>(t));
    }

    auto submit() { return child_.submit(); }

    void cancel() { child_.cancel(); }
};

template<class C> range_context(C&& c)->range_context<C>;
} // namespace detail

template<WriteStreamable S, class F> class transform_write_fn
{
  public:
    template<class C> struct write_context
    {
        C child_context_;

        write_context(C&& c) : child_context_(c) {}

        void submit(write_token&& t)
        {
            child_context_.submit(std::forward<write_token>(t));
        }

        void submit() { child_context_.submit(); }

        void cancel() { child_context_.cancel(); }
    };

  private:
    S stream_;
    F func_;

  public:
    transform_write_fn(S&& stream, F&& f)
        : stream_(std::forward<S>(stream)), func_(std::forward<F>(f))
    {
    }

    template<class V> auto write(V&& v) const
    {
        return write_context<decltype(stream_.write(func_(
            std::forward<V>(v))))>{stream_.write(func_(std::forward<V>(v)))};
    }

    template<ranges::InputRange R> auto write(R&& r) const
    {
        return detail::range_context{
            stream_.write(ranges::view::transform(std::forward<R>(r), func_))};
    }
};

template<ReadStreamable S, class F> class transform_read_fn
{
  public:
    template<class C> struct read_context
    {
        using value_type = decltype(std::declval<C>().submit());

        C                              child_;
        read_done_token<value_type>    done_token_;
        const transform_read_fn<S, F>& stream_;

        read_context(C&& c, const transform_read_fn<S, F>& s)
            : child_(c), stream_(s)
        {
        }

        void done_handler(value_type v) { done_token_(stream_.func_(v)); }

        auto submit() { return stream_.func_(child_.submit()); }

        void submit(read_token<value_type>&& t)
        {
            done_token_  = t.done;
            using this_t = read_context<C>;
            child_.submit(read_token<value_type>{
                t.error, t.cancelled,
                read_done_token<value_type>::template create<
                    this_t, &this_t::done_handler>(this)});
        }

        void cancel() { child_.cancel(); }
    };

  private:
    S stream_;
    F func_;

    template<class C>
    auto make_read_context(C&& c, const transform_read_fn<S, F>& s) const
    {
        return read_context<C>{std::forward<C>(c), s};
    }

  public:
    transform_read_fn(S&& stream, F&& f)
        : stream_(std::forward<S>(stream)), func_(std::forward<F>(f))
    {
    }

    auto read() const { return make_read_context(stream_.read(), *this); }

    auto read(ranges::Range&& r) const
    {
        return detail::range_context{
            stream_.read(output_view::transform(r, func_))};
    }
};

template<WriteStreamable S, class F>
transform_write_fn(S&, F &&)->transform_write_fn<S&, F>;
template<WriteStreamable S, class F>
transform_write_fn(S&&, F &&)->transform_write_fn<S, F>;
template<ReadStreamable S, class F>
transform_read_fn(S&, F &&)->transform_read_fn<S&, F>;
template<ReadStreamable S, class F>
transform_read_fn(S&&, F &&)->transform_read_fn<S, F>;

template<class F> class transform_write_pipe
{
    F f_;

  public:
    transform_write_pipe(F&& f) : f_(f) {}

    template<WriteStreamable S> WriteStreamable pipe(S&& s) const
    {
        return transform_write_fn<S, F>{std::forward<S>(s), F(f_)};
    }
};

template<class F> class transform_read_pipe
{
    F f_;

  public:
    transform_read_pipe(F&& f) : f_(f) {}

    template<ReadStreamable S> ReadStreamable pipe(S&& s) const
    {
        return transform_read_fn<S, F>{std::forward<S>(s), F(f_)};
    }
};

template<class F> Pipeable transform_write(F&& f)
{
    return transform_write_pipe{std::forward<F>(f)};
}

template<class F> Pipeable transform_read(F&& f)
{
    return transform_read_pipe{std::forward<F>(f)};
}

template<WriteStreamable S, class F>
WriteStreamable transform_write(S&& stream, F&& f)
{
    return transform_write_fn<S, F>{std::forward<S>(stream),
                                    std::forward<F>(f)};
}

template<ReadStreamable S, class F>
ReadStreamable transform_read(S&& stream, F&& f)
{
    return transform_read_fn<S, F>{std::forward<S>(stream), std::forward<F>(f)};
}

} // namespace stream

#endif // LIBSTREAM_TRANSFORM_HPP_
