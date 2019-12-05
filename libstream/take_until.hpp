/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_TAKE_UNTIL_HPP_
#define LIBSTREAM_TAKE_UNTIL_HPP_

#include <liboutput_view/take_until.hpp>
#include <libstream/concepts/stream.hpp>
#include <libstream/detail/context.hpp>

namespace stream
{
template<ReadStreamable S, class P> class take_until_read_fn
{
    S stream_;
    P predicate_;

  public:
    take_until_read_fn(S&& stream, P p)
        : stream_(std::forward<S>(stream)), predicate_(std::move(p))
    {
    }

    template<std::experimental::ranges::Range R>
    auto read(R&& r) const requires PureReadStreamable<S>
    {
        return detail::base_range_context{stream_.read(
            output_view::take_until(std::forward<R>(r), predicate_))};
    }

    template<std::experimental::ranges::InputRange Rin,
             std::experimental::ranges::Range      Rout>
    auto readwrite(Rin&& rin, Rout& rout) const requires ReadWriteStreamable<S>
    {
        return detail::base_range_context{stream_.readwrite(
            std::forward<Rin>(rin), output_view::take_until(rout, predicate_))};
    }
};

template<ReadStreamable S, class P>
take_until_read_fn(S&, P)->take_until_read_fn<S&, P>;
template<ReadStreamable S, class P>
take_until_read_fn(S&&, P)->take_until_read_fn<S, P>;

template<ReadStreamable S, class P>
ReadStreamable take_until_read(S&& stream, P&& p)
{
    return take_until_read_fn{std::forward<S>(stream), std::forward<P>(p)};
}
} // namespace stream

#endif /* LIBSTREAM_TAKE_UNTIL_HPP_ */
