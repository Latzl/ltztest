/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#include <iostream>
#include <sstream>
#include "tcbase.hpp"

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
    tcli [options] <command> [args]
Options:
    -h          Show this message and exit.
    -l,         List all command depend on args.
)";

int main(int argc, char* argv[]) {
    std::vector<std::string> vArgs;
    for (int i = 1; i < argc; i++) {
        vArgs.push_back(argv[i]);
    }
    std::size_t argidx = 0;
    for (; argidx < vArgs.size(); argidx++) {
        std::string s = vArgs[argidx];
        if (s[0] == '-') {
            if (s == "-h") {
                std::cout << g_usage << std::endl;
                return 0;
            }
            if (s == "-l") {
                std::vector<std::string> args_l(vArgs.begin() + argidx + 1, vArgs.end());
                tclist(args_l);
                return 0;
            }
        }else{
            break;
        }
    }

    std::vector<std::string> args1(vArgs.begin() + argidx, vArgs.end());
    tc::func func = tc::getRoot().getFunc(args1);
    if (!tc::getRoot().ok_) {
        std::cerr << tc::getRoot().errMsg_ << std::endl;
        exit(-1);
    }
    if (func) {
        func(args1);
    } else {
        tclist(vArgs);
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