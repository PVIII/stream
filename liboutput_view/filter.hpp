/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef LIBOUTPUT_VIEW_FILTER_HPP_
#define LIBOUTPUT_VIEW_FILTER_HPP_

#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/cached_position.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

using namespace std::experimental::ranges;

namespace output_view
{
template<Range V, IndirectUnaryPredicate<iterator_t<V>> Pred>
requires View<V> class filter_view : public view_interface<filter_view<V, Pred>>
{
  private:
    class __iterator;
    class __sentinel;

    V                             base_;
    detail::semiregular_box<Pred> pred_;
    detail::cached_position<V>    begin_;

  public:
    filter_view() = default;

    constexpr filter_view(V base, Pred pred)
        : base_(std::move(base)), pred_(std::move(pred))
    {
    }

    constexpr V base() const { return base_; }

    constexpr __iterator begin()
    {
        return __iterator{*this, __stl2::begin(base_)};
    }

    constexpr __sentinel end() { return __sentinel{*this}; }

    constexpr __iterator end() requires CommonRange<V>
    {
        return __iterator{*this, __stl2::end(base_)};
    }
};

template<Range V, IndirectUnaryPredicate<iterator_t<V>> Pred>
requires View<V> class filter_view<V, Pred>::__iterator
{
  private:
    iterator_t<V> current_{};
    filter_view*  parent_ = nullptr;
    bool          skip_   = false;
    friend __sentinel;

    class output_proxy
    {
        filter_view<V, Pred>::__iterator& parent_;
        iterator_t<V>                     iterator_;

      public:
        output_proxy(filter_view<V, Pred>::__iterator& p, iterator_t<V> i)
            : parent_(p), iterator_(i)
        {
        }

        output_proxy const&
        operator=(iter_value_t<iterator_t<V>> const& value) const
        {
            if(invoke(parent_.parent_->pred_.get(), value))
            { *iterator_ = value; }
            else
            {
                parent_.skip_ = true;
            }
            return *this;
        }
    };

  public:
    using iterator_category = __stl2::output_iterator_tag;
    using value_type        = iter_value_t<iterator_t<V>>;
    using difference_type   = iter_difference_t<iterator_t<V>>;

    __iterator() = default;

    constexpr __iterator(filter_view& parent, iterator_t<V> current)
        : current_(current), parent_(&parent)
    {
    }

    constexpr iterator_t<V> base() const { return current_; }

    constexpr auto operator*() { return output_proxy{*this, current_}; }

    constexpr __iterator& operator++()
    {
        if(!skip_) { ++current_; }
        else
        {
            skip_ = false;
        }
        return *this;
    }

    constexpr void operator++(int)
    {
        if(!skip_) { ++current_; }
        else
        {
            skip_ = false;
        }
    }

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

template<Range V, IndirectUnaryPredicate<iterator_t<V>> Pred>
requires View<V> class filter_view<V, Pred>::__sentinel
{
    sentinel_t<V> end_;

    constexpr bool equal(const __iterator& i) const
    {
        return i.current_ == end_;
    }

  public:
    __sentinel() = default;
    explicit constexpr __sentinel(filter_view& parent)
        : end_(__stl2::end(parent.base_))
    {
    }

    constexpr sentinel_t<V> base() const { return end_; }

    friend constexpr bool operator==(const __iterator& x, const __sentinel& y)
    {
        return y.equal(x);
    }
    friend constexpr bool operator==(const __sentinel& x, const __iterator& y)
    {
        return x.equal(y);
    }
    friend constexpr bool operator!=(const __iterator& x, const __sentinel& y)
    {
        return !y.equal(x);
    }
    friend constexpr bool operator!=(const __sentinel& x, const __iterator& y)
    {
        return !x.equal(y);
    }
};

template<class R, class Pred>
filter_view(R&&, Pred)->filter_view<all_view<R>, Pred>;

struct __filter_fn
{
    template<Range R, IndirectUnaryPredicate<iterator_t<R>> Pred>
    requires ViewableRange<R> constexpr auto operator()(R&&  rng,
                                                        Pred pred) const
    {
        return filter_view{std::forward<R>(rng), std::move(pred)};
    }

    template<CopyConstructible Pred> constexpr auto operator()(Pred pred) const
    {
        return detail::view_closure{*this, std::move(pred)};
    }
}; // namespace output_view

inline constexpr __filter_fn filter{};
} // namespace output_view

#endif /* LIBOUTPUT_VIEW_FILTER_HPP_ */
