#include "tcli.hpp"
#include "tcbase.hpp"

#define TCFUNC_TCLI(...) TCFUNC(tcli, __VA_ARGS__)

void ltz_pp_cat_with_underline() {
    std::cout << "ltz_pp_cat_with_underline" << std::endl;
}
TCFUNC_TCLI(func, ltz_pp_cat_with_underline) {
    ltz_pp_cat_with_underline();
}

// void LTZ_PP_CAT_WITH_UNDERLINE(a, b, c) {}

#include <iostream>
TCLIF(tcli, tclif) {
    std::cout << "tcli/tclif" << std::endl;
    std::cout << tc_file << std::endl;
    std::cout << tc_line << std::endl;
}

TCFUNC_TCLI(tcli_tclif) {
    tcli::func_tree &tree = tcli::get_func_tree();
    auto data = tree.get<tcli::Data *>(tcli::toPath("tcli/tclif"), tcli::to_data_translator());
    data->f({});
}

TCLIF(a, b, c) {
    std::cout << "a/b/c" << std::endl;
}