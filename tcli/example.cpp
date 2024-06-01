#include "tcli_common/tcli.hpp"
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
Note: In cli program, strings behind '--' indicate that are args rather than flag, or options. Because tcli is to simulate cli and it's base on boost promgram_options lib, you will need to pass '-- --' to indicate that strings behind are args for function command, since the first '--' used for tcli and the second '--' used for function command.
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
