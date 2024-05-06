#ifndef TCLI_GTEST_HPP
#define TCLI_GTEST_HPP
#include "../common/tcli_opt.hpp"
#include "../common/tcli.hpp"

#include <gtest/gtest.h>

#define TCLI_F_GTEST(...) TCLI_F(gtest, __VA_ARGS__)

namespace tcli {
namespace gtest {

inline int test_with_filter(const std::string& suit = "", const std::string& test = "") {
    ::testing::InitGoogleTest(&argc, argv);
    std::string filter, suit_ = suit, test_ = test;
    if (suit.empty()) {
        suit_ = "*";
    }
    if (test.empty()) {
        test_ = "*";
    }
    filter = suit_ + "." + test_;
    ::testing::GTEST_FLAG(filter) = filter.c_str();
    return RUN_ALL_TESTS();
}

}  // namespace gtest
}  // namespace tcli

#define TCLI_GTEST_ALL()                                    \
    TCLI_F(gtest) {                                         \
        ::testing::InitGoogleTest(&tcli::argc, tcli::argv); \
        return RUN_ALL_TESTS();                             \
    }

#define TCLI_GTEST_CASE(suit, test)                                                                 \
    TCLI_F_GTEST(suit, test) {                                                                      \
        return ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(suit), BOOST_PP_STRINGIZE(test)); \
    }

#define TCLI_GTEST_SUITE(suit)                                            \
    TCLI_F_GTEST(suit) {                                                  \
        return ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(suit)); \
    }


#endif