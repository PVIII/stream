/*
 * range_matcher.h
 *
 *  Created on: May 16, 2019
 *      Author: patrick
 */

#ifndef TEST_RANGE_MATCHER_H
#define TEST_RANGE_MATCHER_H

#include <experimental/ranges/range>
#include <sstream>

#include <catch2/catch.hpp>

namespace ranges = std::experimental::ranges;

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

#endif // TEST_RANGE_MATCHER_H
