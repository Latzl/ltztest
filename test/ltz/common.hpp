#ifndef TEST_LTZ_COMMON_HPP
#define TEST_LTZ_COMMON_HPP

#include <tcli/tcli.hpp>
#include <tcli/gtest.hpp>

#define FN_LTZ(...) TCLI_FN(ltz, __VA_ARGS__)
#define DF_LTZ(desc, ...) TCLI_DF(desc, ltz, __VA_ARGS__)
#define GF_LTZ(...) TCLI_GTEST_DEF(ltz, __VA_ARGS__)

#endif