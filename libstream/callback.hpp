/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_CALLBACK_HPP_
#define LIBSTREAM_CALLBACK_HPP_

#include <delegate/Delegate.h>

#include <cstddef>

namespace stream
{
using error_code = int;

using error_token                          = SA::delegate<void(error_code)>;
using cancel_token                         = SA::delegate<void()>;
using done_token                           = SA::delegate<void()>;
template<typename T> using read_done_token = SA::delegate<void(T v)>;

template<class... Ret> struct token
{
    error_token                error;
    cancel_token               cancelled;
    SA::delegate<void(Ret...)> done;
};

template<> struct token<void>
{
    error_token          error;
    cancel_token         cancelled;
    SA::delegate<void()> done;
};

using base_token                      = token<>;
template<typename T> using read_token = token<T>;

} // namespace stream

#endif // LIBSTREAM_CALLBACK_HPP_
