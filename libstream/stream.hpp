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

template<class S> concept bool WriteStreamable = requires(S s, typename std::remove_reference_t<S>::value_type v)
{
    {s.write(v).submit()};
};

template<class S> concept bool ReadStreamable = requires(S s)
{
    {s.read().submit()} -> typename std::remove_reference_t<S>::value_type;
};

template<class S>
concept bool Streamable = ReadStreamable<S> || WriteStreamable<S>;

}

#endif // STREAM_HPP_
