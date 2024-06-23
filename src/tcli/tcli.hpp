#ifndef SRC_TCLI_TCLI_HPP
#define SRC_TCLI_TCLI_HPP

#include <tcli/tcli.hpp>

namespace tcli {

int run_op(ltz::proc_init::fn::node& lpif_node);

void list_at(const std::vector<std::string>& vArgsAsFnPath);

void list_all();

void prompt(const std::vector<std::string>& vArgsAsFnPath);

}

#define TCLI_FN_TCLI(...) TCLI_FN(_tcli, __VA_ARGS__)

#endif
