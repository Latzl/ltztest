#include "tcli_gtest.hpp"

TCLI_GTEST_ALL()

TEST(a, 1) {
    std::cout << "a.1" << std::endl;
}
TCLI_GTEST_CASE(a, 1)

TEST(a, 2) {
    std::cout << "a.2" << std::endl;
}
TCLI_GTEST_CASE(a, 2)

TEST(a, 3) {
    std::cout << "a.3" << std::endl;
}
TCLI_GTEST_CASE(a, 3)

TCLI_GTEST_SUITE(a)

TEST(b, 1) {
    std::cout << "b.1" << std::endl;
}
TCLI_GTEST_CASE(b, 1)

TEST(b, 2) {
    std::cout << "b.2" << std::endl;
}
TCLI_GTEST_CASE(b, 2)

TCLI_GTEST_SUITE(b)
