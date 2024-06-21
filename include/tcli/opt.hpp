#ifndef TCLI_OPT_HPP
#define TCLI_OPT_HPP

#include <ltz/proc_init/proc_init.hpp>

namespace tcli {
namespace opt {

struct node : public ltz::proc_init::fn::node {};

ltz::proc_init::fn_reg &get_register();

}  // namespace opt
}  // namespace tcli


/*
    @brief Define function used for options.
    @param ... Variable parameter, which specify the function path.
 */
#define TCLI_OPT_FN(...)                                                \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli_opt, ::tcli::opt::node, __VA_ARGS__); \
    LTZ_PI_FN_DEF_INIT(tcli_opt, __VA_ARGS__) {}                        \
    LTZ_PI_FN_DEF_CLEAN(tcli_opt, __VA_ARGS__) {}                       \
    LTZ_PI_FN_DEF_MAIN(tcli_opt, __VA_ARGS__)

#endif
