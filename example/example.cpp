#include <tcli/tcli.hpp>
#include <iostream>

#define TCLI_FN_EXAMPLE(...) TCLI_FN(example, __VA_ARGS__)
#define TCLI_DF_EXAMPLE(str, ...) TCLI_DF(str, example, __VA_ARGS__)

TCLI_DF_EXAMPLE(R"(Just print hello world.
Usage: path2this)",
    print, hello_world) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}

TCLI_DF_EXAMPLE(R"(Print args.
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
