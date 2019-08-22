/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_PIPE_HPP_
#define LIBSTREAM_PIPE_HPP_

#include <utility>

template<typename P, typename S>
concept bool Pipeable = requires(const P p, S s)
{
	{p.pipe(s)};
};

template<class Pipe, class Stream> auto operator|(const Pipe& pipe, Stream&& stream)
requires Pipeable<Pipe, Stream>
{
    return pipe.pipe(std::forward<Stream>(stream));
}

#endif // LIBSTREAM_PIPE_HPP_
