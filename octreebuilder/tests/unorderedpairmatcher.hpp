#pragma once

#include <gmock/gmock.h>

template <typename PairType>
class UnorderedPairMatcherImpl : public ::testing::MatcherInterface<PairType> {
public:
    typedef GTEST_REMOVE_REFERENCE_AND_CONST_(PairType) RawPairType;
    typedef typename RawPairType::first_type FirstType;
    typedef typename RawPairType::second_type SecondType;

    template <typename FirstMatcher, typename SecondMatcher>
    UnorderedPairMatcherImpl(FirstMatcher first_matcher, SecondMatcher second_matcher)
        : first_matcher_(testing::SafeMatcherCast<const FirstType&>(first_matcher)),
          second_matcher_(testing::SafeMatcherCast<const SecondType&>(second_matcher)) {
    }

    // Describes what this matcher does.
    virtual void DescribeTo(::std::ostream* os) const {
        *os << "has a first field that ";
        first_matcher_.DescribeTo(os);
        *os << ", and has a second field that ";
        second_matcher_.DescribeTo(os);
    }

    // Describes what the negation of this matcher does.
    virtual void DescribeNegationTo(::std::ostream* os) const {
        *os << "has a first field that ";
        first_matcher_.DescribeNegationTo(os);
        *os << ", or has a second field that ";
        second_matcher_.DescribeNegationTo(os);
    }

    // Returns true iff 'a_pair.first' matches first_matcher and 'a_pair.second'
    // matches second_matcher.
    virtual bool MatchAndExplain(PairType a_pair, ::testing::MatchResultListener* listener) const {
        if (!listener->IsInterested()) {
            // If the listener is not interested, we don't need to construct the
            // explanation.
            return (first_matcher_.Matches(a_pair.first) && second_matcher_.Matches(a_pair.second)) ||
                   (first_matcher_.Matches(a_pair.second) && second_matcher_.Matches(a_pair.first));
        }

        ::testing::StringMatchResultListener first_inner_listener;
        ::testing::StringMatchResultListener second_inner_listener;
        if (first_matcher_.MatchAndExplain(a_pair.first, &first_inner_listener) && second_matcher_.MatchAndExplain(a_pair.second, &second_inner_listener)) {
            ExplainSuccess(first_inner_listener.str(), second_inner_listener.str(), listener);
            return true;
        }

        first_inner_listener.Clear();
        second_inner_listener.Clear();
        if (first_matcher_.MatchAndExplain(a_pair.second, &first_inner_listener) && second_matcher_.MatchAndExplain(a_pair.first, &second_inner_listener)) {
            ExplainSuccess(first_inner_listener.str(), second_inner_listener.str(), listener);
            return true;
        }

        ::testing::StringMatchResultListener second_inner_listener_1;
        ::testing::StringMatchResultListener second_inner_listener_2;
        if (!second_matcher_.MatchAndExplain(a_pair.second, &second_inner_listener_1) &&
            !second_matcher_.MatchAndExplain(a_pair.first, &second_inner_listener_2)) {
            *listener << "whose first field does not match";
            first_matcher_.DescribeTo(listener->stream());
            *listener << "and second field does not match";
            second_matcher_.DescribeTo(listener->stream());
            *listener << "or vice versa";
            return false;
        }
        ExplainSuccess(first_inner_listener.str(), second_inner_listener.str(), listener);
        return true;
    }

private:
    void ExplainSuccess(const std::string& first_explanation, const std::string& second_explanation, ::testing::MatchResultListener* listener) const {
        *listener << "whose both fields match";
        if (first_explanation != "") {
            *listener << ", where the first field is a value " << first_explanation;
        }
        if (second_explanation != "") {
            *listener << ", ";
            if (first_explanation != "") {
                *listener << "and ";
            } else {
                *listener << "where ";
            }
            *listener << "the second field is a value " << second_explanation;
        }
    }

    const ::testing::Matcher<const FirstType&> first_matcher_;
    const ::testing::Matcher<const SecondType&> second_matcher_;

    GTEST_DISALLOW_ASSIGN_(UnorderedPairMatcherImpl);
};

// Implements polymorphic Pair(first_matcher, second_matcher).
template <typename FirstMatcher, typename SecondMatcher>
class UnorderedPairMatcher {
public:
    UnorderedPairMatcher(FirstMatcher first_matcher, SecondMatcher second_matcher) : first_matcher_(first_matcher), second_matcher_(second_matcher) {
    }

    UnorderedPairMatcher(const UnorderedPairMatcher& other) : first_matcher_(other.first_matcher_), second_matcher_(other.second_matcher_) {
    }

    template <typename PairType>
    operator ::testing::Matcher<PairType>() const {
        return ::testing::MakeMatcher(new UnorderedPairMatcherImpl<PairType>(first_matcher_, second_matcher_));
    }

private:
    const FirstMatcher first_matcher_;
    const SecondMatcher second_matcher_;

    GTEST_DISALLOW_ASSIGN_(UnorderedPairMatcher);
};

// UnirderedPair(first_matcher, second_matcher) matches a std::pair whose 'first' field
// matches first_matcher and whose 'second' field matches second_matcher or vice versa.
template <typename FirstMatcher, typename SecondMatcher>
UnorderedPairMatcher<FirstMatcher, SecondMatcher> UnorderedPair(FirstMatcher first_matcher, SecondMatcher second_matcher) {
    return UnorderedPairMatcher<FirstMatcher, SecondMatcher>(first_matcher, second_matcher);
}
