/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_DEMULTIPLEX_HPP_
#define LIBSTREAM_DEMULTIPLEX_HPP_

#include <libstream/callback.hpp>
#include <libstream/concepts/stream.hpp>

#include <experimental/ranges/range>

#include <utility>

namespace stream
{
namespace detail
{
template<class C1, class C2> class write_context
{
    C1         child1_;
    C2         child2_;
    base_token done_token_;
    bool       first_child_;

    void done_handler()
    {
        first_child_ = false;
        child2_.submit(std::move(done_token_));
    }

  public:
    write_context(C1 c1, C2 c2) : child1_(c1), child2_(c2) {}

    void submit()
    {
        child1_.submit();
        child2_.submit();
    }

    void submit(base_token t)
    {
        done_token_  = std::move(t);
        first_child_ = true;
        using this_t = write_context<C1, C2>;
        child1_.submit(base_token{
            t.error, t.cancelled,
            done_token::template create<this_t, &this_t::done_handler>(this)});
    }

    void cancel()
    {
        if(first_child_) { child1_.cancel(); }
        else
        {
            child2_.cancel();
        }
    }
};

template<class C1, class C2> write_context(C1&&, C2 &&)->write_context<C1, C2>;
} // namespace detail
template<PureWriteStreamable S1, PureWriteStreamable S2> class demultiplex_fn
{
    S1 stream1_;
    S2 stream2_;

  public:
    demultiplex_fn(S1&& s1, S2&& s2)
        : stream1_(std::forward<S1>(s1)), stream2_(std::forward<S2>(s2))
    {
    }

    template<class V> auto write(V v) const
    {
        return detail::write_context{stream1_.write(v), stream2_.write(v)};
    }

    template<std::experimental::ranges::InputRange R> auto write(R& r) const
    {
        return detail::write_context{stream1_.write(r), stream2_.write(r)};
    }
};

template<PureWriteStreamable S1, PureWriteStreamable S2>
demultiplex_fn(S1&&, S2 &&)->demultiplex_fn<S1, S2>;

template<PureWriteStreamable S1, PureWriteStreamable S2>
PureWriteStreamable demultiplex(S1&& s1, S2&& s2)
{
    return demultiplex_fn{std::forward<S1>(s1), std::forward<S2>(s2)};
}
} // namespace stream

#endif /* LIBSTREAM_DEMULTIPLEX_HPP_ */
