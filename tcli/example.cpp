#include "tcli.hpp"
#include <iostream>

#define TCLIF_EXAMPLE(...) TCLIF(example, __VA_ARGS__)

TCLIF_EXAMPLE(print, hello_world) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}

TCLIF_EXAMPLE(add) {
    int a, b;
    a = std::stoi(tcArgs[0]);
    b = std::stoi(tcArgs[1]);
    std::cout << a + b << std::endl;
    return 0;
}

TCLIF_EXAMPLE(set, return_value){
    return std::stoi(tcArgs[0]);
}