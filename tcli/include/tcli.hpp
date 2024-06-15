/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#ifndef TCLI_HPP
#define TCLI_HPP


#include <string>
#include <vector>

#include <ltz/proc_init/proc_init.hpp>
#include <tcli/utility.hpp>

namespace tcli {

extern int argc_raw;
extern char** argv_raw;
extern std::vector<std::string> args_pass2fn, args_fn_path;

struct node : ltz::proc_init::fn::node {
    std::string desc;
};

void list(const std::vector<std::string>& vArgsAsFnPath);

void list_all();

void prompt(const std::vector<std::string>& vArgsAsFnPath);

namespace ipc {
int listen();
int connect();
}  // namespace ipc

int main(int argc, char* argv[]);

}  // namespace tcli

#define TCLI_GET_REG() LTZ_PI_FN_GET_REG(tcli)
#define TCLI_FN(...)                                           \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::node, __VA_ARGS__); \
    LTZ_PI_FN_DEF_INIT(tcli, __VA_ARGS__) {}                   \
    LTZ_PI_FN_DEF_CLEAN(tcli, __VA_ARGS__) {}                  \
    LTZ_PI_FN_DEF_MAIN(tcli, __VA_ARGS__)

#define TCLI_DF(_desc, ...)                                    \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::node, __VA_ARGS__); \
    LTZ_PI_FN_NODE_HANDLE(tcli, set_desc, __VA_ARGS__) {       \
        auto& node = dynamic_cast<tcli::node&>(lpif_node);     \
        node.desc = _desc;                                     \
    }                                                          \
    LTZ_PI_FN_DEF_INIT(tcli, __VA_ARGS__) {}                   \
    LTZ_PI_FN_DEF_CLEAN(tcli, __VA_ARGS__) {}                  \
    LTZ_PI_FN_DEF_MAIN(tcli, __VA_ARGS__)

#endif
