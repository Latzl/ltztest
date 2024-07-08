#include "opt.hpp"
#include "tcli.hpp"

namespace tcli {
namespace opt {

ltz::proc_init::fn_reg& get_register() {
    auto& reg = LTZ_PI_FN_GET_REG(tcli_opt);
    return reg;
}

Opt opt;

void Opt::init(std::vector<std::string> vArgs) {
    /* run all options register define on other source */
    auto& reg = get_register();
    reg.run_all();

    /* init parser */
    pparser_ = new boost::program_options::command_line_parser(vArgs);
    for (auto& opt_desc : opt_desc_list_) {
        opt_desc_.add(opt_desc);
    }
    pparser_->options(opt_desc_).positional(opt_pos_desc_);
}

void Opt::parse() {
    pparsed_opts_ = new boost::program_options::parsed_options(pparser_->run());
    boost::program_options::store(*pparsed_opts_, vm_);
    boost::program_options::notify(vm_);
}

Opt& Opt::add_description(const boost::program_options::options_description& desc) {
    opt_desc_list_.push_back(desc);
    return *this;
}

Opt& Opt::add_pos_description(const std::string& name, int max_count) {
    opt_pos_desc_.add(name.c_str(), max_count);
    return *this;
}

std::string Opt::help() {
    std::stringstream ss;
    ss << opt_desc_;
    return ss.str();
}

void add_description(const boost::program_options::options_description& desc){
    opt.add_description(desc);
}

/* define options of tcli */
TCLI_OPT_FN(tcli) {
    boost::program_options::options_description desc{"tcli_opt"};
    desc.add_options()  //
        ("help,h", boost::program_options::bool_switch(), "Show this message then exit.")  //
        ("list,t", boost::program_options::bool_switch(), "List sub path of current given path then exit.")  //
        ("list-all,T", boost::program_options::bool_switch(), "List all registered function tree then exit. Node with t{f} indicate that a function has registered on this node; Therefore, the path to this node can be the path to excutable function. Node with t{g} indicate that a function description has add on this node.")  //
        ("fpath,f", boost::program_options::value<std::vector<std::string>>()->default_value(args_fn_path, "")->multitoken(), "Set function path to execute.")  //
        ("print-desc,p", boost::program_options::bool_switch(), "Print the corresponding description of the function. Only TCLI_DF() specified by function path was used will take effect.")("silence,s", boost::program_options::bool_switch(), "Silence mode(not implement yet).")  //
        ("verbose,v", boost::program_options::bool_switch(), "Verbose mode(not implement yet).")  //
        ("listen,l", boost::program_options::bool_switch(), "Listen other tcli process. As tcli server, wait for function path message from client then parse this path and execute.")  //
        ("connect,c", boost::program_options::bool_switch(), "Connect to tcli server. Send function path message to server.")  //
        ;
    auto& opt = tcli::opt::opt;
    opt.add_description(desc);
    opt.add_pos_description("fpath", -1);
    return 0;
}
}  // namespace opt
}  // namespace tcli
