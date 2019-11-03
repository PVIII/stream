/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_FILTER_HPP_
#define LIBSTREAM_FILTER_HPP_

#include <liboutput_view/filter.hpp>
#include <libstream/concepts/stream.hpp>
#include <libstream/detail/context.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
namespace detail
{
template<class C> struct write_filter_context
{
    bool empty_;
    union {
        detail::base_write_context<C> base_context_;
        detail::empty_write_context   empty_context_;
    };
    void submit(write_token&& t)
    {
        if(empty_) { empty_context_.submit(std::move(t)); }
        else
        {
            base_context_.submit(std::forward<write_token>(t));
        }
    }
    void submit() noexcept
    {
        if(empty_) { empty_context_.submit(); }
        else
        {
            base_context_.submit();
        }
    }
    void cancel() noexcept
    {
        if(empty_) { empty_context_.cancel(); }
        else
        {
            base_context_.cancel();
        }
    }
};

template<class C> write_filter_context(bool, C&& c)->write_filter_context<C>;

template<class C, class S>
class read_filter_context : public base_read_context<C>
{
    using typename base_read_context<C>::value_type;
    using base_read_context<C>::child_;
    S& stream_;

    read_token<value_type> token_;

    void submit_internal()
    {
        using this_t = read_filter_context<C, S>;
        child_.submit(
            read_token<value_type>{token_.error, token_.cancelled,
                                   read_done_token<value_type>::template create<
                                       this_t, &this_t::done_handler>(this)});
    }

    void done_handler(value_type v)
    {
        if(stream_.predicate_(v)) { token_.done(v); }
        else
        {
            submit_internal();
        }
    }

  public:
    read_filter_context(C&& c, S& s)
        : base_read_context<C>{std::forward<C>(c)}, stream_(s)
    {
    }

    auto submit() noexcept
    {
        while(true)
        {
            auto result = child_.submit();
            if(stream_.predicate_(result)) { return result; }
        }
    }

    void submit(read_token<value_type>&& t)
    {
        token_ = t;
        submit_internal();
    }
};

template<class C, class S>
read_filter_context(C&& c, S& s)->read_filter_context<C, S>;
} // namespace detail

template<WriteStreamable S, class P> class filter_write_fn
{
    S stream_;
    P predicate_;

  public:
    filter_write_fn(S&& stream, P&& p)
        : stream_(std::forward<S>(stream)), predicate_(std::forward<P>(p))
    {
    }

    template<class V> auto write(V&& v) const
    {
        if(predicate_(v))
        {
            return detail::write_filter_context{
                false, stream_.write(std::forward<V>(v))};
        }
        else
        {
            return detail::write_filter_context<decltype(
                stream_.write(std::forward<V>(v)))>{true, {}};
        }
    }

    template<ranges::InputRange R> auto write(R&& r) const
    {
        return detail::base_range_context{stream_.write(
            ranges::view::filter(std::forward<R>(r), predicate_))};
    }

    template<ranges::InputRange Rin, ranges::Range Rout>
    auto readwrite(Rin&& rin, Rout&& rout) const requires ReadWriteStreamable<S>
    {
        return detail::base_range_context{stream_.readwrite(
            ranges::view::filter(std::forward<Rin>(rin), predicate_),
            std::forward<Rout>(rout))};
    }
};

template<WriteStreamable S, class P>
filter_write_fn(S&, P &&)->filter_write_fn<S&, P>;
template<WriteStreamable S, class P>
filter_write_fn(S&&, P &&)->filter_write_fn<S, P>;

template<WriteStreamable S, class P>
WriteStreamable filter_write(S&& stream, P&& p)
{
    return filter_write_fn{std::forward<S>(stream), std::forward<P>(p)};
}

template<ReadStreamable S, class P> class filter_read_fn
{
    S stream_;
    P predicate_;

  public:
    filter_read_fn(S&& stream, P&& p)
        : stream_(std::forward<S>(stream)), predicate_(std::forward<P>(p))
    {
    }

    auto read() const requires PureReadStreamable<S>
    {
        return detail::read_filter_context{stream_.read(), *this};
    }

    auto read(ranges::Range& r) const requires PureReadStreamable<S>
    {
        return detail::base_range_context{
            stream_.read(output_view::filter(r, predicate_))};
    }

    template<class V>
    auto readwrite(V&& v) const requires ReadWriteStreamable<S>
    {
        return detail::read_filter_context{
            stream_.readwrite(std::forward<V>(v)), *this};
    }

    template<ranges::InputRange Rin, ranges::Range Rout>
    auto readwrite(Rin&& rin, Rout& rout) const requires ReadWriteStreamable<S>
    {
        return detail::base_range_context{stream_.readwrite(
            std::forward<Rin>(rin), output_view::filter(rout, predicate_))};
    }
};

template<ReadStreamable S, class P>
filter_read_fn(S&, P &&)->filter_read_fn<S&, P>;
template<ReadStreamable S, class P>
filter_read_fn(S&&, P &&)->filter_read_fn<S, P>;

template<ReadStreamable S, class P>
ReadStreamable filter_read(S&& stream, P&& p)
{
    return filter_read_fn{std::forward<S>(stream), std::forward<P>(p)};
}
} // namespace stream

#endif /* LIBSTREAM_FILTER_HPP_ */
