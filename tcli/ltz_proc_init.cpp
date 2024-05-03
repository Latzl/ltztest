#include "../third_party/ltz/proc_init/proc_init.hpp"
#include <iostream>

#include "tcli.hpp"

#define LPI_TEST(...) LTZ_PI_F(test, __VA_ARGS__)
#define LPI_STET(...) LTZ_PI_F(stet, __VA_ARGS__)
#define TCLIF_LPI(...) TCLI_F(lpi, __VA_ARGS__)

LPI_TEST() {
    std::cout << "lpi_test: (empty)" << std::endl;
    return 0;
}
TCLIF_LPI(test, empty) {
    auto &reg = ltz::proc_init::get_register("test");
    reg.run("");
    return 0;
}

LPI_TEST(a, b) {
    std::cout << "lpi_test: a, b" << std::endl;
    return 0;
}

LPI_TEST(a, b, c) {
    std::cout << "lpi_test: a, b, c" << std::endl;
    return 0;
}

LPI_TEST(a, b, d) {
    std::cout << "lpi_test: a, b, d" << std::endl;
    return 0;
}

TCLIF_LPI(test) {
    auto &reg = ltz::proc_init::get_register("test");
    reg.run(lpiArgs[0]);
    return 0;
}

TCLIF_LPI(test, all) {
    auto &reg = ltz::proc_init::get_register("test");
    reg.run_all();
    return 0;
}

LPI_STET(1, 2, 3) {
    std::cout << "lpi_stet: 1, 2, 3" << std::endl;
    return 0;
}

LPI_STET(a, 2, 3) {
    std::cout << "lpi_stet: a, 2, 3" << std::endl;
    return 0;
}

TCLIF_LPI(stet) {
    auto &reg = ltz::proc_init::get_register("stet");
    reg.run(lpiArgs[0]);
    return 0;
}