/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_CONCEPTS_STREAM_HPP_
#define LIBSTREAM_CONCEPTS_STREAM_HPP_

#include <type_traits>

namespace stream
{
template<class S> concept bool PureWriteStreamable = requires
{
    &std::remove_reference_t<S>::write;
};

template<class S> concept bool PureReadStreamable = requires(S s)
{
    {s.read().submit()};
};

template<class S> concept bool ReadWriteStreamable = requires(S s)
{
    {std::remove_reference_t<S>::readwrite};
};

template<class S>
concept bool ReadStreamable = PureReadStreamable<S> || ReadWriteStreamable<S>;
template<class S>
concept bool WriteStreamable = PureWriteStreamable<S> || ReadWriteStreamable<S>;

template<class S>
concept bool Streamable =
    PureReadStreamable<S> || PureWriteStreamable<S> || ReadWriteStreamable<S>;

} // namespace stream

#endif // LIBSTREAM_CONCEPTS_STREAM_HPP_
