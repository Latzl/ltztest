#ifndef TCLI_OPT_HPP
#define TCLI_OPT_HPP

#include "../third_party/ltz/proc_init/proc_init.hpp"
#include <boost/program_options.hpp>

#define TCLI_OPT_F(...) LTZ_PI_F(tcli_opt, __VA_ARGS__)

namespace tcli {
namespace opt {

class Opt {
   private:
    Opt() = default;
    ~Opt() = default;

   private:
    boost::program_options::command_line_parser* pparser_{nullptr};
    boost::program_options::parsed_options* pparsed_opts_{nullptr};
    boost::program_options::options_description opt_desc_;
    std::vector<boost::program_options::options_description> opt_desc_list_;
    boost::program_options::positional_options_description opt_pos_desc_;

   public:
    boost::program_options::variables_map vm_;

   public:
    inline static Opt& instance() {
        static Opt opt;
        return opt;
    }
    inline void init(int argc, char* argv[]) {
        /* run all options register define on other source */
        auto& reg = ltz::proc_init::get_register("tcli_opt");
        reg.silence_ = true;
        reg.run_all();

        /* init parser */
        pparser_ = new boost::program_options::command_line_parser(argc, argv);
        for (auto& opt_desc : opt_desc_list_) {
            opt_desc_.add(opt_desc);
        }
        pparser_->options(opt_desc_).positional(opt_pos_desc_).allow_unregistered();
    }
    inline void parse() {
        pparsed_opts_ = new boost::program_options::parsed_options(pparser_->run());
        boost::program_options::store(*pparsed_opts_, vm_);
        boost::program_options::notify(vm_);
    }

    inline Opt& add_description(const boost::program_options::options_description& desc) {
        opt_desc_list_.push_back(desc);
        return *this;
    }

    inline Opt& add_pos_description(const std::string& name, int max_count) {
        opt_pos_desc_.add(name.c_str(), max_count);
        return *this;
    }
};
}  // namespace opt
}  // namespace tcli

#endif
