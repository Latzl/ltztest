#ifndef SRC_TCLI_OPT_HPP
#define SRC_TCLI_OPT_HPP

#include <tcli/opt.hpp>
#include <boost/program_options.hpp>

namespace tcli {
namespace opt {

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

#endif