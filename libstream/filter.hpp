/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_FILTER_HPP_
#define LIBSTREAM_FILTER_HPP_

#include <libstream/concepts/stream.hpp>
#include <libstream/detail/context.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<WriteStreamable S, class P> class filter_write_fn
{
  public:
    template<class C> struct optional_context
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

  private:
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
            return optional_context<decltype(stream_.write(
                std::forward<V>(v)))>{false, stream_.write(std::forward<V>(v))};
        }
        else
        {
            return optional_context<decltype(
                stream_.write(std::forward<V>(v)))>{true, {}};
        }
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
} // namespace stream

#endif /* LIBSTREAM_FILTER_HPP_ */
