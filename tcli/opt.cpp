#include <tcli/opt.hpp>

namespace tcli {
namespace opt {

Opt opt;

void Opt::init(std::vector<std::string> vArgs) {
    /* run all options register define on other source */
    auto& reg = TCLI_OPT_GET_REG();
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
}  // namespace opt
}  // namespace tcli
