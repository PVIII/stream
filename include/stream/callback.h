#ifndef STREAM_CALLBACK_H_
#define STREAM_CALLBACK_H_

#include <cstddef>

#include <delegate/Delegate.h>

namespace stream
{
using error_code = int;

using completion_token = SA::delegate<void(error_code, std::size_t n)>;
} // namespace stream

#endif // STREAM_CALLBACK_H_
