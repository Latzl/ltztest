#include <tcli/gtest.hpp>

TCLI_GTEST_CALL_ALL()

TCLI_GTEST_DEF(a, b, c, a, 1) {
    std::cout << "a.1" << std::endl;
}
TCLI_GTEST_CALL_CASE(a, b, c, a, 1)

TCLI_GTEST_DEF(a, b, c, a, 2) {
    std::cout << "a.2" << std::endl;
}
TCLI_GTEST_CALL_CASE(a, b, c, a, 2)

TCLI_GTEST_DEF(a, b, c, a, 3) {
    std::cout << "a.3" << std::endl;
}
TCLI_GTEST_CALL_CASE(a, b, c, a, 3)

TCLI_GTEST_CALL_SUITE(a, b, c, a)

TCLI_GTEST_DEF(x, y, z, b, 1) {
    std::cout << "b.1" << std::endl;
}
TCLI_GTEST_CALL_CASE(x, y, z, b, 1)

TCLI_GTEST_DEF(x, y, z, b, 2) {
    std::cout << "b.2" << std::endl;
}
TCLI_GTEST_CALL_CASE(x, y, z, b, 2)

TCLI_GTEST_CALL_SUITE(x, y, z, b)
