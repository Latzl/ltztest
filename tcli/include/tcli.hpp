/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#ifndef TCLI_HPP
#define TCLI_HPP

#include <string>
#include <vector>
#include <iostream>

#include <ltz/proc_init/proc_init.hpp>

#define TCLI_F(...) LTZ_PI_F(tcli, __VA_ARGS__)

#define TCLI_P(str, ...)   \
    LTZ_PI_HANDLE_REG_OBJ( \
        tcli, [](ltz::proc_init::Data& data) { data.desc_ = str; }, __VA_ARGS__)

namespace tcli {

extern int argc;
extern char** argv;
extern std::vector<std::string> args;

ltz::proc_init::Register& get_register();

void list(const std::vector<std::string>& v_path);

void list_all();

void prompt(const std::vector<std::string>& v);

namespace ipc {
int listen();
int connect();
}  // namespace ipc

int main(int argc, char* argv[]);

}  // namespace tcli

#endif
