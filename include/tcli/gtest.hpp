#ifndef TCLI_GTEST_HPP
#define TCLI_GTEST_HPP

#include <tcli/tcli.hpp>
#include <tcli/opt.hpp>

#include <gtest/gtest.h>

#define TCLI_FN_GTEST(...) TCLI_FN(gtest, __VA_ARGS__)

namespace tcli {
namespace gtest {

int test_with_filter(const std::string& suit = "", const std::string& test = "");

}  // namespace gtest
}  // namespace tcli

#define TCLI_GTEST_ALL()                                            \
    TCLI_FN(gtest) {                                                \
        ::testing::InitGoogleTest(&tcli::argc_raw, tcli::argv_raw); \
        return RUN_ALL_TESTS();                                     \
    }

#define TCLI_GTEST_CASE(suit, test)                                                                 \
    TCLI_FN_GTEST(suit, test) {                                                                     \
        return ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(suit), BOOST_PP_STRINGIZE(test)); \
    }

#define TCLI_GTEST_SUITE(suit)                                            \
    TCLI_FN_GTEST(suit) {                                                 \
        return ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(suit)); \
    }


#endif