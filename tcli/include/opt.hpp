#ifndef TCLI_OPT_HPP
#define TCLI_OPT_HPP

#include <ltz/proc_init/proc_init.hpp>
#include <boost/program_options.hpp>

// #define TCLI_OPT_F(...) LTZ_PI_F(tcli_opt, __VA_ARGS__)
namespace tcli {
namespace opt {

struct node : public ltz::proc_init::fn::node {};

class Opt {
   public:
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
    void init(std::vector<std::string> vArgs);
    void parse();

    Opt& add_description(const boost::program_options::options_description& desc);

    Opt& add_pos_description(const std::string& name, int max_count);

    std::string help();
};

extern Opt opt;
}  // namespace opt
}  // namespace tcli

#define TCLI_OPT_GET_REG() LTZ_PI_FN_GET_REG(tcli_opt)
#define TCLI_OPT_FN(...)                                                \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli_opt, ::tcli::opt::node, __VA_ARGS__); \
    LTZ_PI_FN_DEF_INIT(tcli_opt, __VA_ARGS__) {}                        \
    LTZ_PI_FN_DEF_CLEAN(tcli_opt, __VA_ARGS__) {}                       \
    LTZ_PI_FN_DEF_MAIN(tcli_opt, __VA_ARGS__)

#endif
