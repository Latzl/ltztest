#include "common/tcli.hpp"
#include <iostream>

#define TCLI_F_EXAMPLE(...) TCLI_F(example, __VA_ARGS__)

TCLI_F_EXAMPLE(print, hello_world) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
