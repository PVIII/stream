/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_CONCEPTS_PIPE_HPP_
#define LIBSTREAM_CONCEPTS_PIPE_HPP_

#include <libstream/concepts/stream.hpp>

#include <utility>

namespace stream
{
template<class P> concept bool Pipeable = requires()
{
    {std::remove_reference_t<P>::pipe};
};

template<Streamable S, Pipeable P> concept bool PipeableTo = requires(S s, P p)
{
    {p.pipe(s)};
};

template<Streamable S, Pipeable P>
constexpr Streamable operator|(S&& stream, const P& pipe) requires PipeableTo<S, P>
{
    return pipe.pipe(std::forward<S>(stream));
}

template<Pipeable P1, Pipeable P2> class pure_pipe
{
    P1 p1_;
    P2 p2_;

  public:
    pure_pipe(P1&& p1, P2&& p2) : p1_(p1), p2_(p2) {}

    template<Streamable S> Streamable pipe(S&& s) const
    {
        return p2_.pipe(p1_.pipe(std::forward<S>(s)));
    }

    template<Pipeable P> Pipeable pipe(P&& p) const
    {
        return pure_pipe{*this, std::forward<P>(p)};
    }
};

template<Pipeable P1, Pipeable P2> Pipeable operator|(P1&& p1, P2&& p2)
{
    return pure_pipe<P1, P2>{std::forward<P1>(p1), std::forward<P2>(p2)};
}

} // namespace stream

#endif // LIBSTREAM_CONCEPTS_PIPE_HPP_
