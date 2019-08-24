/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_PIPE_HPP_
#define LIBSTREAM_PIPE_HPP_

#include <utility>

template<class S> concept bool WriteStream = requires(S s)
{
    {s.write(0).submit()};
};

template<class S> concept bool ReadStream = requires(S s)
{
    {s.read().submit()};
};

template<class S> concept bool Stream = ReadStream<S> || WriteStream<S>;

template<class P> concept bool Pipe = requires(const P p) { {P::pipe}; };

template<Pipe P, Stream S> concept bool Pipeable = requires(const P p, S s)
{
    {p.pipe(s)};
};

template<Stream S, Pipe P>
auto operator|(S&& stream, const P& pipe) requires Pipeable<P, S>
{
    return pipe.pipe(std::forward<S>(stream));
}

template<Pipe P1, Pipe P2> class pure_pipe
{
    P1 p1_;
    P2 p2_;

  public:
    pure_pipe(P1&& p1, P2&& p2) : p1_(p1), p2_(p2) {}

    template<Stream S> auto pipe(S&& s) const
    {
        return p2_.pipe(p1_.pipe(std::forward<S>(s)));
    }
};

template<Pipe P1, Pipe P2> Pipe operator|(P1&& p1, P2&& p2)
{
    return pure_pipe{std::forward<P1>(p1), std::forward<P2>(p2)};
}

#endif // LIBSTREAM_PIPE_HPP_
