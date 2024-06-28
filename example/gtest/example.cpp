#include <tcli/gtest.hpp>

TCLI_GTEST_ALL()

TCLI_GTEST(a, b, c, a, 1) {
    std::cout << "a.1" << std::endl;
}
TCLI_GTEST_CASE(a, b, c, a, 1)

TCLI_GTEST(a, b, c, a, 2) {
    std::cout << "a.2" << std::endl;
}
TCLI_GTEST_CASE(a, b, c, a, 2)

TCLI_GTEST(a, b, c, a, 3) {
    std::cout << "a.3" << std::endl;
}
TCLI_GTEST_CASE(a, b, c, a, 3)

TCLI_GTEST_SUITE(a, b, c, a)

TCLI_GTEST(x, y, z, b, 1) {
    std::cout << "b.1" << std::endl;
}
TCLI_GTEST_CASE(x, y, z, b, 1)

TCLI_GTEST(x, y, z, b, 2) {
    std::cout << "b.2" << std::endl;
}
TCLI_GTEST_CASE(x, y, z, b, 2)

TCLI_GTEST_SUITE(x, y, z, b)
