#include <tcli/tcli.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#define FN_EXAMPLE(...) TCLI_FN(example, __VA_ARGS__)
#define DF_EXAMPLE(str, ...) TCLI_DF(str, example, __VA_ARGS__)

DF_EXAMPLE(R"(Just print hello world.
Usage: path2this)",
    print, hello_world) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}

FN_EXAMPLE(directly_use, by_reg, print, hello_world) {
    return LTZ_PI_FN_GET_REG(tcli).run("example/print/hello_world");
}

FN_EXAMPLE(directly_use, by_macro, print, hello_world) {
    return TCLI_RF({}, example, print, hello_world);
}

DF_EXAMPLE(R"(Print args.
    Usage: path2this [--] [{arg}[ {arg1}[ ...]]])",
    print, args) {
    for (auto s : lpif_args) {
        std::cout << s << std::endl;
    }
    return 0;
}

FN_EXAMPLE(directly_use, by_macro, print, args) {
    return TCLI_RF(lpif_args, example, print, args);
}