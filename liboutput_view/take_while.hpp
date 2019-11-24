/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBOUTPUT_VIEW_TAKE_WHILE_HPP_
#define LIBOUTPUT_VIEW_TAKE_WHILE_HPP_

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

namespace std::experimental::ranges
{
namespace ext::output
{
template<Range V, class Pred>
requires View<V> class take_while_view
    : public view_interface<take_while_view<V, Pred>>
{
  private:
    class __iterator;
    class __sentinel;

    V                             base_;
    detail::semiregular_box<Pred> pred_;

  public:
    take_while_view() = default;

    constexpr take_while_view(V base, Pred pred)
        : base_(std::move(base)), pred_(std::move(pred))
    {
    }

    constexpr V base() const { return base_; }

    constexpr __iterator begin()
    {
        return __iterator{*this, __stl2::begin(base_)};
    }

    constexpr __sentinel end() { return __sentinel{*this}; }
};

template<View V, class Pred> class take_while_view<V, Pred>::__iterator
{
  private:
    iterator_t<V>    current_{};
    take_while_view* parent_               = nullptr;
    bool             predicate_been_false_ = false;
    friend __sentinel;

    class output_proxy
    {
        take_while_view<V, Pred>::__iterator& parent_;
        iterator_t<V>&                        iterator_;

      public:
        output_proxy(take_while_view<V, Pred>::__iterator& p, iterator_t<V>& i)
            : parent_(p), iterator_(i)
        {
        }

        output_proxy const& operator=(auto const& value) const
        {
            if(invoke(parent_.parent_->pred_.get(), value))
            { *iterator_ = value; }
            else
            {
                parent_.predicate_been_false_ = true;
            }
            return *this;
        }
    };

  public:
    using iterator_category = __stl2::output_iterator_tag;
    using value_type        = iter_value_t<iterator_t<V>>;
    using difference_type   = iter_difference_t<iterator_t<V>>;

    __iterator() = default;

    constexpr __iterator(take_while_view& parent, iterator_t<V> current)
        : current_(current), parent_(&parent)
    {
    }

    constexpr iterator_t<V> base() const { return current_; }

    constexpr auto operator*() { return output_proxy{*this, current_}; }

    constexpr __iterator& operator++()
    {
        ++current_;
        return *this;
    }

    constexpr void operator++(int) { ++current_; }

    friend constexpr bool
    operator==(const __iterator& x,
               const __iterator& y) requires EqualityComparable<iterator_t<V>>
    {
        return x.current_ == y.current_;
    }

    friend constexpr bool
    operator!=(const __iterator& x,
               const __iterator& y) requires EqualityComparable<iterator_t<V>>
    {
        return !(x == y);
    }
};

template<Range V, class Pred>
requires View<V> class take_while_view<V, Pred>::__sentinel
{
    sentinel_t<V> end_;

    constexpr bool equal(const __iterator& i) const
    {
        return i.current_ == end_;
    }

  public:
    __sentinel() = default;
    explicit constexpr __sentinel(take_while_view& parent)
        : end_(__stl2::end(parent.base_))
    {
    }

    constexpr sentinel_t<V> base() const { return end_; }

    friend constexpr bool operator==(const __iterator& x, const __sentinel& y)
    {
        return y.equal(x) || x.predicate_been_false_;
    }
    friend constexpr bool operator==(const __sentinel& x, const __iterator& y)
    {
        return x.equal(y) || y.predicate_been_false_;
    }
    friend constexpr bool operator!=(const __iterator& x, const __sentinel& y)
    {
        return !(y == x);
    }
    friend constexpr bool operator!=(const __sentinel& x, const __iterator& y)
    {
        return !(x == y);
    }
};

template<class R, class Pred>
take_while_view(R&&, Pred)->take_while_view<all_view<R>, Pred>;

struct __take_while_fn
{
    template<Range R, class Pred>
    requires ViewableRange<R> constexpr auto operator()(R&&  rng,
                                                        Pred pred) const
    {
        return take_while_view{std::forward<R>(rng), std::move(pred)};
    }

    template<CopyConstructible Pred> constexpr auto operator()(Pred pred) const
    {
        return detail::view_closure{*this, std::move(pred)};
    }
};

} // namespace ext::output
} // namespace std::experimental::ranges

namespace output_view
{
inline constexpr std::experimental::ranges::ext::output::__take_while_fn
    take_while{};
} // namespace output_view

#endif /* LIBOUTPUT_VIEW_FILTER_HPP_ */
