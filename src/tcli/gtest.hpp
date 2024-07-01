#ifndef SRC_TCLI_GTEST_HPP
#define SRC_TCLI_GTEST_HPP

#include <tcli/gtest.hpp>

namespace tcli {
namespace gtest {

namespace lpif = ltz::proc_init::fn;

bool is_need_take_over();
int main();

enum class test_type {
    unknown,
    all,
    suit,
    case_,
    subs,
};

std::string toStr(test_type type);

class Filter {
    using lpif_reg = ltz::proc_init::fn_reg;
    /* construct helper */
   private:
    bool trans2all();
    bool trans2case();
    bool trans2suit();
    bool trans2subs();

   public:
    Filter(const std::vector<std::string> &path);

    bool ready() const;
    std::string get_filter() const;
    test_type get_type() const;

   private:
    std::string filter_;
    test_type type_{test_type::unknown};
    bool bReady_{false};

    /* helper */
   private:
    const std::vector<std::string> nodePath;
    std::vector<std::string>::const_iterator itRoot_, itSuit_, itCase_;

    lpif_reg::reg_p::reg_tree *pRegTree_{nullptr};
};

int test_with_filter(const std::string& filter);
int test_with_filter(const std::string& suit_name, const std::string& case_name);

}  // namespace gtest
}  // namespace tcli

#endif  // SRC_TCLI_GTEST_HPP