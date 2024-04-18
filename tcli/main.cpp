/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#include <iostream>
#include <sstream>
#include "tcbase.hpp"

#include <boost/program_options.hpp>
namespace bpo = boost::program_options;

void tclist(const std::vector<std::string>& args) {
    std::vector<std::string> args1 = args;
    tc::getRoot().listLastCmdOption(args1);
    if (!tc::getRoot().ok_) {
        std::cerr << tc::getRoot().errMsg_ << std::endl;
        exit(-1);
    }
}

const std::string g_usage = R"(
Usage:
    tcli [options] <func path> [args]
Options:
    --help,-h                   Show this message and exit.
    --list,-l                   List all command depend on args.
    --fpath,-f=<func path>      Set function path to be executed.
    --silence,-s                Silence mode.
    --verbose,-v                Verbose mode.
)";
bool g_bSilence = false;
bool g_bVerbose = false;

int main(int argc, char* argv[]) {
    std::vector<std::string> funcpath;
    try {
        bpo::options_description desc{"Options"};
        desc.add_options()  //
            ("help,h", "Show this message and exit.")  //
            ("list,l", bpo::bool_switch(), "List all command depend on args.")  //
            ("fpath,f", bpo::value<std::vector<std::string>>()->default_value(funcpath, "")->multitoken(), "Set function path to be executed.")  //
            ("silence,s", bpo::bool_switch(&g_bSilence), "Silence mode.")  //
            ("verbose,v", bpo::bool_switch(&g_bVerbose), "Verbose mode.");

        bpo::positional_options_description posDesc;
        posDesc.add("fpath", -1);

        bpo::command_line_parser parser(argc, argv);
        parser.options(desc).positional(posDesc);
        bpo::parsed_options parsedOpts = parser.run();

        bpo::variables_map vm;
        bpo::store(parsedOpts, vm);
        bpo::notify(vm);

        if (vm.count("help")) {
            std::cout << g_usage << std::endl;
            return 0;
        }
        funcpath = vm["fpath"].as<std::vector<std::string>>();
        if (vm["list"].as<bool>()) {
            std::vector<std::string> funcpath2List(funcpath.begin(), funcpath.end());
            tclist(funcpath);
            return 0;
        }
    } catch (const bpo::error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::vector<std::string> funcpath2parse(funcpath.begin(), funcpath.end());
    tc::func func = tc::getRoot().getFunc(funcpath2parse);
    if (!tc::getRoot().ok_) {
        std::cerr << tc::getRoot().errMsg_ << std::endl;
        exit(-1);
    }
    if (func) {
        func(funcpath2parse);
    } else {
        tclist(funcpath);
    }
}

/* define test function below */

TCFUNC(a, b, c) {
    std::cout << "a-b-c" << std::endl;
}

TCFUNC(a, b) {
    std::cout << "a-b" << std::endl;
}

TCFUNC(a, b, d) {
    std::cout << "a-b-d" << std::endl;
}

TCFUNC(d, e, f, g) {
    std::cout << "d-e-f-g" << std::endl;
}