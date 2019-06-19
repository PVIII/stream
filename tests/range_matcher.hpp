/*
 * range_matcher.h
 *
 *  Created on: May 16, 2019
 *      Author: patrick
 */

#ifndef TESTS_RANGE_MATCHER_HPP_
#define TESTS_RANGE_MATCHER_HPP_

#include <catch2/catch.hpp>

#include <experimental/ranges/range>
#include <sstream>

namespace ranges = std::experimental::ranges;

namespace stream
{
template<ranges::Range R> class RangeMatcher : public Catch::MatcherBase<R>
{
    R range_;

  public:
    RangeMatcher(R&& r) : range_(r) {}

    bool match(R const& r) const override { return ranges::equal(range_, r); }

    std::string describe() const override
    {
        std::ostringstream ss;
        ss << "equals [";
        for(auto const& v : range_) { ss << v << ", "; }
        ss << "]";
        return ss.str();
    }
};

template<ranges::Range R> inline RangeMatcher<R> Equals(R&& r)
{
    return RangeMatcher<R>(std::forward<R>(r));
}

} // namespace stream

#endif // TESTS_RANGE_MATCHER_HPP_
