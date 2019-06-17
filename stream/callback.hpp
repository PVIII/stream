#ifndef STREAM_CALLBACK_HPP_
#define STREAM_CALLBACK_HPP_

#include <delegate/Delegate.h>

#include <cstddef>

namespace stream
{
using error_code = int;

using completion_token = SA::delegate<void(error_code, std::size_t n)>;
} // namespace stream

#endif // STREAM_CALLBACK_HPP_
