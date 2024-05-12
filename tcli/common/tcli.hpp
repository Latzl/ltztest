#ifndef TCLI_HPP
#define TCLI_HPP

#include <string>
#include <vector>
#include <iostream>

#include "../../third_party/ltz/proc_init/proc_init.hpp"

#define TCLI_F(...) LTZ_PI_F(tcli, __VA_ARGS__)

namespace tcli {

extern int argc;
extern char** argv;
extern std::vector<std::string> args;

inline ltz::proc_init::Register& get_register() {
    return ltz::proc_init::get_register("tcli");
}

inline void list(const std::vector<std::string>& v_path) {
    std::string s = get_register().list_children(v_path.begin(), v_path.end());
    if (s.size()) {
        std::cout << s << std::endl;
    }
}

inline void list_all() {
    std::string s = get_register().toStr_registered(0, "tcli");
    if (s.size()) {
        std::cout << s << std::endl;
    }
}

namespace ipc {
int listen();
int connect();
}  // namespace ipc

int main();

}  // namespace tcli

#endif
