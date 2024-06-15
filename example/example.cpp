#include <tcli/tcli.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#define TCLI_FN_EXAMPLE(...) TCLI_FN(example, __VA_ARGS__)
#define TCLI_DF_EXAMPLE(str, ...) TCLI_DF(str, example, __VA_ARGS__)

TCLI_DF_EXAMPLE(R"(Just print hello world.
Usage: path2this)",
    print, hello_world) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}

TCLI_DF_EXAMPLE(R"(Print args.
    Usage: path2this [--] [{arg}[ {arg1}[ ...]]])",
    print, args) {
    for (auto s : lpif_args) {
        std::cout << s << std::endl;
    }
    return 0;
}

TCLI_FN_EXAMPLE(add) {
    int a, b;
    a = std::stoi(lpif_args[0]);
    b = std::stoi(lpif_args[1]);
    std::cout << a + b << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(set, return_value) {
    return std::stoi(lpif_args[0]);
}

TCLI_FN_EXAMPLE(a, b_c) {
    std::cout << "a, b_c" << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(a_b, c) {
    std::cout << "a_b, c" << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(a, b) {
    std::cout << "a, b" << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(a, b, c) {
    std::cout << "a, b, c" << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(ab) {
    std::cout << "ab" << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(ab, cd) {
    std::cout << "ab/cd" << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(ab, cd, ef) {
    std::cout << "ab/cd/ef" << std::endl;
    return 0;
}

TCLI_FN_EXAMPLE(ab, cd, ef, gh) {
    std::cout << "ab/cd/ef/gh" << std::endl;
    return 0;
}

TCLI_FN(print, hello, world) {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

TCLI_FN(directly_use, print_hello_world) {
    return LTZ_PI_FN_GET_REG(tcli).run("print/hello/world");
}