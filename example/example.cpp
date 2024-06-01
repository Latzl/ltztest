#include <tcli/tcli.hpp>
#include <iostream>

#define TCLI_F_EXAMPLE(...) TCLI_F(example, __VA_ARGS__)
#define TCLI_P_EXAMPLE(str, ...) TCLI_P(str, example, __VA_ARGS__)

TCLI_F_EXAMPLE(print, hello_world) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}

TCLI_P_EXAMPLE(R"(Just print hello world.
Usage: path2this)",
    print, hello_world)

TCLI_F_EXAMPLE(print, args) {
    for (auto s : lpiArgs) {
        std::cout << s << std::endl;
    }
    return 0;
}

TCLI_P_EXAMPLE(R"(Print args.
Usage: path2this [-- --] [{arg}[ {arg1}[ ...]]]
Note: In cli program, strings behind '--' indicate that are args rather than flag, or options. Because tcli is to simulate cli and it's base on boost promgram_options lib, you will need to pass '-- --' to indicate that strings behind are args for test command, since the first '--' used for tcli and the second '--' used for test command.
    For example, the use 'tcli example print args -- -- foo bar' will print:
        foo
        bar
    in which won't print the second '--'. If you want use '--' as first arg, you will need:
        'tcli example print args -- -- -- foo bar'
    in which will print:
        --
        foo
        bar)",
    print, args)

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
