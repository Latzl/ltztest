#include <tcli/gtest.hpp>

namespace test {

int return1() {
    return 1;
}
int return2() {
    return 2;
}

namespace arithmetic {

int add(int a, int b) {
    return a + b;
}

int sub(int a, int b) {
    return a - b;
}
}  // namespace arithmetic

namespace logic {

bool land(bool a, bool b) {
    return a && b;
}

bool lor(bool a, bool b) {
    return a || b;
}
}  // namespace logic
}  // namespace test

TCLI_GTEST_FN(test, return1) {
    EXPECT_EQ(test::return1(), 1);
}

TCLI_GTEST_FN(test, return2) {
    EXPECT_EQ(test::return2(), 2);
}

TCLI_GTEST_FN(test, arithmetic, add) {
    EXPECT_EQ(test::arithmetic::add(1, 2), 3);
    EXPECT_EQ(test::arithmetic::add(-1, -2), -3);

    EXPECT_EQ(test::arithmetic::add(INT32_MIN, INT32_MIN), 0);
    EXPECT_EQ(test::arithmetic::add(INT32_MIN, INT32_MAX), -1);
    EXPECT_EQ(test::arithmetic::add(INT32_MAX, INT32_MIN), -1);
    EXPECT_EQ(test::arithmetic::add(INT32_MAX, INT32_MAX), -2);
}

TCLI_GTEST_FN(test, arithmetic, sub) {
    EXPECT_EQ(test::arithmetic::sub(1, 2), -1);
    EXPECT_EQ(test::arithmetic::sub(-1, -2), 1);

    EXPECT_EQ(test::arithmetic::sub(INT32_MIN, INT32_MIN), 0);
    EXPECT_EQ(test::arithmetic::sub(INT32_MIN, INT32_MAX), 1);
    EXPECT_EQ(test::arithmetic::sub(INT32_MAX, INT32_MIN), -1);
    EXPECT_EQ(test::arithmetic::sub(INT32_MAX, INT32_MAX), 0);
}

TCLI_GTEST_FN(test, logic, land) {
    EXPECT_EQ(test::logic::land(false, false), false);
    EXPECT_EQ(test::logic::land(false, true), false);
    EXPECT_EQ(test::logic::land(true, false), false);
    EXPECT_EQ(test::logic::land(true, true), true);
}

TCLI_GTEST_FN(test, logic, lor) {
    EXPECT_EQ(test::logic::lor(false, false), false);
    EXPECT_EQ(test::logic::lor(false, true), true);
    EXPECT_EQ(test::logic::lor(true, false), true);
    EXPECT_EQ(test::logic::lor(true, true), true);
}

TCLI_GTEST_FN(other, a, b) {
    std::cout << "other/a/b" << std::endl;
}

TCLI_GTEST_FN(other, a, c) {
    std::cout << "other/a/c" << std::endl;
}

TCLI_GTEST_FN(other, b, a) {
    std::cout << "other/b/a" << std::endl;
}

TCLI_GTEST_FN(other, b, c) {
    std::cout << "other/b/c" << std::endl;
}

TCLI_GTEST_FN(other, b, d) {
    std::cout << "other/b/d" << std::endl;
}