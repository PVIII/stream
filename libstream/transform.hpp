/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBSTREAM_TRANSFORM_HPP_
#define LIBSTREAM_TRANSFORM_HPP_

#include <liboutput_view/transform.hpp>
#include <libstream/callback.hpp>

#include <experimental/ranges/range>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<class Stream, class F> class transform
{
  public:
    template<class C> struct range_context
    {
        C child_;

        range_context(C&& c) : child_(c) {}

        void submit(completion_token&& t)
        {
            child_.submit(std::forward<completion_token>(t));
        }

        auto submit() { return child_.submit(); }
    };
    template<class T, class C> struct read_context
    {
        C                     child_context_;
        read_token<T>         token_;
        transform<Stream, F>& stream_;

        read_context(C&& c, transform<Stream, F>& s)
            : child_context_(c), stream_(s)
        {
        }

        void handler(error_code ec, T v) { token_(ec, stream_.func_(v)); }

        void submit(read_token<T>&& t)
        {
            token_       = t;
            using this_t = read_context<T, C>;
            child_context_.submit(
                read_token<T>::template create<this_t, &this_t::handler>(this));
        }
    };
    template<class C> struct write_context
    {
        C child_context_;

        write_context(C&& c) : child_context_(c) {}

        void submit(write_token&& t)
        {
            child_context_.submit(std::forward<write_token>(t));
        }

        void submit() { child_context_.submit(); }
    };

  private:
    Stream& stream_;
    F       func_;

    template<class T, class C>
    auto make_read_context(C&& c, transform<Stream, F>& s)
    {
        return read_context<T, C>{std::forward<C>(c), s};
    }

  public:
    transform(Stream& stream, F&& f) : stream_(stream), func_(f) {}

    auto write(auto const& v) { return write_context{stream_.write(func_(v))}; }

    template<ranges::Range R> auto write(R&& r)
    {
        return range_context{
            stream_.write(ranges::view::transform(std::forward<R>(r), func_))};
    }

    auto read() const { return func_(stream_.read()); }

    template<class T> auto read()
    {
        return make_read_context<T>(stream_.read_single(), *this);
    }

    auto read(ranges::Range&& r)
    {
        return range_context{stream_.read(output_view::transform(r, func_))};
    }
};
} // namespace stream

#endif // LIBSTREAM_TRANSFORM_HPP_
