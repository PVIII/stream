/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef STREAM_HPP_
#define STREAM_HPP_

#include <type_traits>

namespace stream
{

template<class S> concept bool WriteStreamable = requires(S s)
{
    {std::remove_reference_t<S>::write};
};

template<class S> concept bool ReadStreamable = requires(S s)
{
    {s.read().submit()};
};

template<class S>
concept bool Streamable = ReadStreamable<S> || WriteStreamable<S>;

}

#endif // STREAM_HPP_
