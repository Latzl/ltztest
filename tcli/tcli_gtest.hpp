#ifndef TCLI_GTEST_HPP
#define TCLI_GTEST_HPP
#include "tcli_opt.hpp"
#include "tcli.hpp"

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

#define TCLI_GTEST_TEST_ALL() ::tcli::gtest::test_with_filter()
#define TCLI_GTEST_TEST_FILTER(suit, test) ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(suit), BOOST_PP_STRINGIZE(test))
#define TCLI_GTEST_TEST_SUITE(suit) ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(suit))


#endif