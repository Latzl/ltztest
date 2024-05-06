#include "common/tcli.hpp"
#include <iostream>

#define TCLI_F_EXAMPLE(...) TCLI_F(example, __VA_ARGS__)

TCLI_F_EXAMPLE(print, hello_world) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}

TCLI_F_EXAMPLE(add) {
    int a, b;
    a = std::stoi(lpiArgs[0]);
    b = std::stoi(lpiArgs[1]);
    std::cout << a + b << std::endl;
    return 0;
}

TCLI_F_EXAMPLE(set, return_value){
    return std::stoi(lpiArgs[0]);
}

TCLI_F_EXAMPLE(a, b_c) {
    std::cout << "a, b_c" << std::endl;
    return 0;
}

TCLI_F_EXAMPLE(a_b, c) {
    std::cout << "a_b, c" << std::endl;
    return 0;
}

TCLI_F_EXAMPLE(a, b) {
    std::cout << "a, b" << std::endl;
    return 0;
}

TCLI_F_EXAMPLE(a, b, c) {
    std::cout << "a, b, c" << std::endl;
    return 0;
}
