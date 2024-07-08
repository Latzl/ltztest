#include <tcli/opt.hpp>

// todo process third options in tcli::main
TCLI_OPT_FN(example) {
    boost::program_options::options_description desc{"example_opt"};
    desc.add_options()  //
        ("example,e", boost::program_options::bool_switch()->notifier([](bool b) {
            if (b) {
                std::cout << "example" << std::endl;
                exit(0);
            }
        }),
            "print 'example' to stdout")  //
        ;
    tcli::opt::add_description(desc);
    return 0;
}