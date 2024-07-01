#ifndef SRC_TCLI_TCLI_HPP
#define SRC_TCLI_TCLI_HPP

#include <tcli/tcli.hpp>

namespace tcli {

int run_op(ltz::proc_init::fn::node& lpif_node);

std::vector<std::string> get_registered_nodes_at(const std::vector<std::string>& vArgsAsFnPath);

std::string toStr_candidate_nodes(const std::vector<std::string> &vCandidates);

void list_at(const std::vector<std::string>& vArgsAsFnPath);

void list_all();

void print_desc(const std::vector<std::string>& vArgsAsFnPath);

}

#define TCLI_FN_TCLI(...) TCLI_FN(_tcli, __VA_ARGS__)

#endif
