#ifndef LIBSTREAM_CALLBACK_HPP_
#define LIBSTREAM_CALLBACK_HPP_

#include <delegate/Delegate.h>

#include <cstddef>

namespace stream
{
using error_code = int;

using completion_token = SA::delegate<void(error_code, std::size_t n)>;
} // namespace stream

#endif // LIBSTREAM_CALLBACK_HPP_
