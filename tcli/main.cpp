/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "tcli.hpp"

#include <boost/program_options.hpp>
namespace bpo = boost::program_options;

int tcli::argc = 0;
char** tcli::argv = nullptr;

void tcli_list(const std::vector<std::string>& vPath) {
    std::string s = tcli::get_children_name(tcli::toPath(vPath.begin(), vPath.end()));
    if (s.size()) {
        std::cout << s << std::endl;
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
    tcli::argc = argc;
    tcli::argv = (char**)argv;

    std::vector<std::string> vArgs;
    try {
        bpo::options_description desc{"Options"};
        desc.add_options()  //
            ("help,h", "Show this message and exit.")  //
            ("list,l", bpo::bool_switch(), "List all command depend on args.")  //
            ("fpath,f", bpo::value<std::vector<std::string>>()->default_value(vArgs, "")->multitoken(), "Set function path to be executed.")  //
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
        vArgs = vm["fpath"].as<std::vector<std::string>>();
        if (vm["list"].as<bool>()) {
            tcli_list(vArgs);
            return 0;
        }
    } catch (const bpo::error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    auto pr = tcli::get_data(vArgs.begin(), vArgs.end());
    if (!pr.first) {
        std::cerr << "no such path: " << tcli::toPath(vArgs.begin(), vArgs.end()) << std::endl;
        tcli_list(vArgs);
        return -1;
    }

    auto tStart = std::chrono::steady_clock::now();
    int r = pr.first->f(std::vector<std::string>(pr.second, vArgs.end()));
    auto tEnd = std::chrono::steady_clock::now();

    /* print return value, time cost */
    {
        auto tDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(tEnd - tStart);
        double ns = tDiff.count();
        std::string unit;
        if (ns < 1e3) {
            unit = "ns";
        } else if (ns < 1e6) {
            ns /= 1e3;
            unit = "us";
        } else if (ns < 1e9) {
            ns /= 1e6;
            unit = "ms";
        } else {
            ns /= 1e9;
            unit = "s";
        }
        std::cout << "=======" << std::endl;
        std::cout << "return value: " << r << std::endl;
        std::cout << "time cost: " << std::fixed << std::setprecision(2) << ns << unit << std::endl;
    }

    return r;
}

/* define test function below */

TCLIF(print_tree) {
    tcli::print_tree();
    return 0;
}

TCLIF(echo_args) {
    for (const auto& s : tcArgs) {
        std::cout << s << std::endl;
    }
    return 0;
}

TCLIF(a, b_c) {
    std::cout << "a, b_c" << std::endl;
    return 0;
}

TCLIF(a_b, c) {
    std::cout << "a_b, c" << std::endl;
    return 0;
}
