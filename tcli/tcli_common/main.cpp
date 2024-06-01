/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#include "tcli_opt.hpp"
#include "tcli.hpp"

int main(int argc, char* argv[]) {
    tcli::argc = argc;
    tcli::argv = (char**)argv;

    auto& opt = tcli::opt::Opt::instance();
    opt.init(argc, argv);
    opt.parse();

    auto& vm = opt.vm_;

    /* tcli */
    tcli::args = vm["fpath"].as<std::vector<std::string>>();
    if (vm["list"].as<bool>()) {
        tcli::list(tcli::args);
        return 0;
    }
    if (vm["list-all"].as<bool>()) {
        tcli::list_all();
        return 0;
    }
    if (vm["prompt"].as<bool>()){
        tcli::prompt(tcli::args);
        return 0;
    }
    if (vm["listen"].as<bool>()) {
        return tcli::ipc::listen();
    }
    if (vm["connect"].as<bool>()) {
        return tcli::ipc::connect();
    }
    if (vm["help"].as<bool>() || tcli::args.empty()) {
        std::cout << opt.get_help() << std::endl;
        std::cout << "registered fuction tree: " << std::endl;
        tcli::list_all();
        return 0;
    }
    return tcli::main();
}
