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
using range_done_token                     = SA::delegate<void(std::size_t)>;
using done_token                           = SA::delegate<void()>;
template<typename T> using read_done_token = SA::delegate<void(T v)>;

struct completion_token
{
    error_token      error;
    cancel_token     cancelled;
    range_done_token done;
};

struct write_token
{
    error_token  error;
    cancel_token cancelled;
    done_token   done;
};

template<typename T> struct read_token
{
    error_token        error;
    cancel_token       cancelled;
    read_done_token<T> done;
};

} // namespace stream

#endif // LIBSTREAM_CALLBACK_HPP_
