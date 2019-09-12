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

using error_token = SA::delegate<void(error_code)>;
using done_token  = SA::delegate<void(std::size_t)>;

struct completion_token
{
    error_token error;
    done_token  done;
};

using write_token                     = SA::delegate<void(error_code)>;
template<typename T> using read_token = SA::delegate<void(error_code, T v)>;

} // namespace stream

#endif // LIBSTREAM_CALLBACK_HPP_
