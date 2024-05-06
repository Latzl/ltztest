#include "tcli.hpp"
#include "tcli_opt.hpp"

namespace tcli {

int argc = 0;
char** argv = nullptr;
std::vector<std::string> args;

/* define options of tcli */
TCLI_OPT_F(tcli) {
    boost::program_options::options_description desc("tcli");
    desc.add_options()  //
        ("help,h", boost::program_options::bool_switch(), "Show this message and exit.")  //
        ("list,t", boost::program_options::bool_switch(), "List sub path of current given path.")  //
        ("list-all,T", boost::program_options::bool_switch(), "List all register function.")  //
        ("fpath,f", boost::program_options::value<std::vector<std::string>>()->default_value(args, "")->multitoken(), "Set function path to execute.")  //
        ("silence,s", boost::program_options::bool_switch(), "Silence mode.")  //
        ("verbose,v", boost::program_options::bool_switch(), "Verbose mode.")  //
        ;
    auto& opt = tcli::opt::Opt::instance();
    opt.add_description(desc);
    opt.add_pos_description("fpath", -1);
    return 0;
}

}  // namespace tcli

/* define function below */
TCLI_F(prereg, toStr_registered_debug) {
    std::cout << tcli::get_register().toStr_registered(1, "root") << std::endl;
    return 0;
}

TCLI_F(prereg, echo_args) {
    for (const auto& s : lpiArgs) {
        std::cout << s << std::endl;
    }
    return 0;
}