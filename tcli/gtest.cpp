#include "tcli_gtest.hpp"

TEST(a, 1) {
    std::cout << "a.1" << std::endl;
}

TCLI_F_GTEST(a, 1) {
    return TCLI_GTEST_TEST_FILTER(a, 1);
}

TEST(a, 2) {
    std::cout << "a.2" << std::endl;
}

TCLI_F_GTEST(a, 2) {
    return TCLI_GTEST_TEST_FILTER(a, 2);
}

TEST(a, 3) {
    std::cout << "a.3" << std::endl;
}

TCLI_F_GTEST(a, 3) {
    return TCLI_GTEST_TEST_FILTER(a, 3);
}

TCLI_F_GTEST(a) {
    return TCLI_GTEST_TEST_SUITE(a);
}

TEST(b, 1) {
    std::cout << "b.1" << std::endl;
}

TCLI_F_GTEST(b, 1) {
    return TCLI_GTEST_TEST_FILTER(b, 1);
}

TEST(b, 2) {
    std::cout << "b.2" << std::endl;
}

TCLI_F_GTEST(b, 2) {
    return TCLI_GTEST_TEST_FILTER(b, 2);
}

TCLI_F_GTEST(b) {
    return TCLI_GTEST_TEST_SUITE(b);
}

TCLI_F(gtest) {
    return TCLI_GTEST_TEST_ALL();
}