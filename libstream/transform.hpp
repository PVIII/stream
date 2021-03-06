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
#include <libstream/concepts/pipe.hpp>
#include <libstream/detail/context.hpp>

#include <experimental/ranges/range>

namespace stream
{
namespace detail
{
template<class C, class S> class read_context : public base_read_context<C>
{
    using typename base_read_context<C>::value_type;
    using base_read_context<C>::child_;
    S& stream_;

    read_done_token<value_type> done_token_;

    void done_handler(value_type v) { done_token_(stream_.func_(v)); }

  public:
    read_context(C&& c, S& s)
        : base_read_context<C>{std::forward<C>(c)}, stream_(s)
    {
    }

    auto submit() { return stream_.func_(child_.submit()); }

    void submit(read_token<value_type>&& t)
    {
        done_token_  = t.done;
        using this_t = read_context<C, S>;
        child_.submit(
            read_token<value_type>{t.error, t.cancelled,
                                   read_done_token<value_type>::template create<
                                       this_t, &this_t::done_handler>(this)});
    }
};

template<class C, class S> read_context(C&& c, S& s)->read_context<C, S>;
} // namespace detail

template<WriteStreamable S, class F> class transform_write_fn
{
    S stream_;
    F func_;

  public:
    transform_write_fn(S&& stream, F&& f)
        : stream_(std::forward<S>(stream)), func_(std::forward<F>(f))
    {
    }

    template<class V> auto write(V&& v) const
    {
        return detail::base_write_context{
            stream_.write(func_(std::forward<V>(v)))};
    }

    template<std::experimental::ranges::InputRange R> auto write(R&& r) const
    {
        return detail::base_range_context{
            stream_.write(std::experimental::ranges::view::transform(
                std::forward<R>(r), func_))};
    }

    template<class V>
    auto readwrite(V&& v) const requires ReadWriteStreamable<S>
    {
        return detail::base_read_context{stream_.readwrite(func_(v))};
    }

    template<std::experimental::ranges::InputRange Rin,
             std::experimental::ranges::Range      Rout>
    auto readwrite(Rin&& rin, Rout&& rout) const requires ReadWriteStreamable<S>
    {
        return detail::base_range_context{
            stream_.readwrite(std::experimental::ranges::view::transform(
                                  std::forward<Rin>(rin), func_),
                              std::forward<Rout>(rout))};
    }
};

template<ReadStreamable S, class F> class transform_read_fn
{
  private:
    S stream_;
    F func_;

  public:
    transform_read_fn(S&& stream, F&& f)
        : stream_(std::forward<S>(stream)), func_(std::forward<F>(f))
    {
    }

    auto read() const requires PureReadStreamable<S>
    {
        return detail::read_context{stream_.read(), *this};
    }

    auto read(std::experimental::ranges::Range&& r) const
        requires PureReadStreamable<S>
    {
        return detail::base_range_context{
            stream_.read(output_view::transform(r, func_))};
    }

    template<class V>
    auto readwrite(V&& v) const requires ReadWriteStreamable<S>
    {
        return detail::read_context{stream_.readwrite(std::forward<V>(v)),
                                    *this};
    }

    template<std::experimental::ranges::InputRange Rin,
             std::experimental::ranges::Range      Rout>
    auto readwrite(Rin&& rin, Rout&& rout) const requires ReadWriteStreamable<S>
    {
        return detail::base_range_context{stream_.readwrite(
            std::forward<Rin>(rin), output_view::transform(rout, func_))};
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
