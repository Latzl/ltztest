#ifndef TCLI_HPP
#define TCLI_HPP

#include <string>
#include <vector>
#include <iostream>

#include <boost/preprocessor.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../third_party/ltz/proc_init/proc_init.hpp"

#define TCLI_F(...) LTZ_PI_F(tcli, __VA_ARGS__)

namespace tcli {

extern int argc;
extern char** argv;

inline ltz::proc_init::Register &get_register() {
    return ltz::proc_init::get_register("tcli");
}
}  // namespace tcli

#endif
