#ifndef TCLI_HPP
#define TCLI_HPP

#include <string>
#include <vector>

#include <boost/preprocessor.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../third_party/ltz/preprocessor/preprocessor.hpp"

namespace tcli {

using args_container = std::vector<std::string>;

extern int argc;
extern char **argv;

struct Data {
    virtual void f(const std::vector<std::string> &tcArgs) = 0;
    std::string tc_desc;
    std::string tc_file;
    int tc_line{0};
};

struct to_data_translator {
    typedef Data *internal_type;
    typedef Data *external_type;
    boost::optional<external_type> get_value(internal_type p) {
        return p;
    }
    boost::optional<internal_type> put_value(external_type p) {
        return p;
    }
};

using func_tree = boost::property_tree::basic_ptree<std::string, Data *>;

inline func_tree &get_func_tree() {
    static func_tree tree;
    return tree;
}

inline std::string toPath(std::string path, char delimiter = '/') {
    std::replace(path.begin(), path.end(), delimiter, '.');
    return path;
}

inline std::string toPath(const args_container &v) {
    std::string path;
    for (auto &s : v) {
        path += toPath(s) + ".";
    }
    if (path.size()) {
        path.pop_back();
    }
    return path;
}

inline std::string get_children_name(const std::string &path) {
    auto opt = get_func_tree().get_child_optional(toPath(path));
    if (!opt) {
        return "";
    }
    std::string s;
    for (auto pr : *opt) {
        s += pr.first + "\n";
    }
    if (s.size()) {
        s.pop_back();
    }
    return s;
}

inline std::pair<Data *, args_container::iterator> get_data(args_container::iterator itl, args_container::iterator itr, func_tree &tree = get_func_tree()) {
    if (itl > itr) {
        return {nullptr, itr};
    }
    if (itl == itr) {
        return {tree.get_value<Data *>(to_data_translator()), itl};
    }
    auto opt = tree.get_child_optional(*itl);
    if (!opt) {
        return {tree.get_value<Data *>(to_data_translator()), itl};
    }
    return get_data(itl + 1, itr, *opt);
}

}  // namespace tcli

#define TCLI_GEN_NAME(prefix, ...) BOOST_PP_CAT(prefix##_, LTZ_PP_CAT_WITH_UNDERLINE(__VA_ARGS__))
#define TCLI_GEN_NAME_REG(...) TCLI_GEN_NAME(_tcli_reg, __VA_ARGS__)
#define TCLI_GEN_NAME_REG_OBJ(...) TCLI_GEN_NAME(tcli_reg_obj, __VA_ARGS__)

#define TCLIF(...)                                                                              \
    namespace tcli {                                                                            \
    struct TCLI_GEN_NAME_REG(__VA_ARGS__) : public Data {                                       \
        TCLI_GEN_NAME_REG(__VA_ARGS__)() {                                                      \
            std::string func_path = BOOST_PP_STRINGIZE(LTZ_PP_CAT_WITH_UNDERLINE(__VA_ARGS__)); \
            func_path = toPath(func_path, '_');                                                 \
            tc_file = __FILE__;                                                                 \
            tc_line = __LINE__;                                                                 \
            get_func_tree().put(func_path, this, to_data_translator());                         \
        }                                                                                       \
        void f(const std::vector<std::string> &tcArgs) override;                                \
    } TCLI_GEN_NAME_REG_OBJ(__VA_ARGS__);                                                       \
    }                                                                                           \
    void tcli::TCLI_GEN_NAME_REG(__VA_ARGS__)::f(const std::vector<std::string> &tcArgs)

#endif