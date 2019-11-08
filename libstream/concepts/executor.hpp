/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_CONCEPTS_EXECUTOR_HPP_
#define LIBSTREAM_CONCEPTS_EXECUTOR_HPP_

#include <experimental/ranges/concepts>

namespace stream
{
template<class F, class Ret, class... Args>
concept bool AsyncInvokable = requires(F f, Args... args)
{
    {
        f(args...).submit(token<Ret>{})
    }
    ->void;
};

template<class F, class Ret, class... Args>
concept bool SyncInvokable = requires(F f, Args... args)
{
    {
        f(args...).submit()
    }
    ->Ret;
};

template<class F, class Ret, class... Args>
concept bool Executable =
    AsyncInvokable<F, Ret, Args...> || SyncInvokable<F, Ret, Args...>;

} // namespace stream

#endif // LIBSTREAM_CONCEPTS_EXECUTOR_HPP_
