/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_DETAIL_CONTEXT_HPP_
#define LIBSTREAM_DETAIL_CONTEXT_HPP_

#include <libstream/callback.hpp>

#include <utility>

namespace stream
{
namespace detail
{
struct empty_write_context
{
    void submit(write_token&& t) { t.done(); }
    void submit() noexcept {}
    void cancel() noexcept {}
};

template<class C> struct base_write_context
{
    C child_context_;

    base_write_context(C&& c) : child_context_(c) {}

    void submit(write_token&& t)
    {
        child_context_.submit(std::forward<write_token>(t));
    }

    void submit() { child_context_.submit(); }

    void cancel() { child_context_.cancel(); }
};

template<class C> base_write_context(C &&)->base_write_context<C>;

template<class C> struct base_range_context
{
    C child_;

    void submit(completion_token&& t)
    {
        child_.submit(std::forward<completion_token>(t));
    }

    auto submit() { return child_.submit(); }

    void cancel() { child_.cancel(); }
};

template<class C> base_range_context(C&& c)->base_range_context<C>;

} // namespace detail
} // namespace stream

#endif /* LIBSTREAM_DETAIL_CONTEXT_HPP_ */
