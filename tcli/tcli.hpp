#ifndef TCLI_HPP
#define TCLI_HPP

#include <string>
#include <vector>
#include <iostream>

#include <boost/preprocessor.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../third_party/ltz/preprocessor/preprocessor.hpp"

namespace tcli {

using args_container = std::vector<std::string>;

extern int argc;
extern char **argv;

struct Data {
    virtual int f(const std::vector<std::string> &tcArgs) = 0;
    std::string desc_;
    std::string file_;
    int line_{0};
};

struct to_data_translator {
    using internal_type = Data *;
    using external_type = Data *;
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

template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
inline std::string toPath(It itl, It itr) {
    std::string path;
    while (itl != itr) {
        path += *itl + ".";
        itl = std::next(itl);
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

inline void print_tree_impl(func_tree &tree, int depth = 0) {
    for (auto &pr : tree) {
        const std::string key_child = pr.first;
        func_tree &tree_child = pr.second;
        Data *data_child = tree_child.get_value<Data *>();
        std::stringstream ss;
        ss << "|" << std::string(depth * 2, '-') << key_child << "(0x" << std::hex << (long int)&tree_child << ", 0x" << (long int)data_child << ")";
        std::cout << ss.str() << std::endl;
        print_tree_impl(tree_child, depth + 1);
    }
}

/*
    @brief print tree
    @details print tree and children, format like:
        tree(this, ptr to data)
        |--child_key(child_this, ptr to child_data)
        |----key(0x123456, 0xabcdef)
        ...
    @note use put_data to ensure ptr to data of path node is null
 */
inline void print_tree(func_tree &tree = get_func_tree()) {
    Data *data = tree.get_value<Data *>();
    std::stringstream ss;
    ss << "tree(0x" << std::hex << (long int)&tree << ", 0x" << (long int)data << ")";
    std::cout << ss.str() << std::endl;
    print_tree_impl(tree, 1);
}

template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
inline std::pair<Data *, It> get_data(It itl, It itr, func_tree &tree = get_func_tree()) {
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
    return get_data(std::next(itl), itr, *opt);
}

/*
    @brief put ptr to data
    @note prefer to use this function rather than boost::property_tree:put to ensure ptr to data of path node is null
 */
template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
inline func_tree &put_data(It itl, It itr, Data *data = nullptr, func_tree &tree = get_func_tree()) {
    if (itl > itr) {
        return tree;
    }
    if (itl == itr) {
        tree.put_value(data, to_data_translator());
        return tree;
    }
    auto opt = tree.get_child_optional(*itl);
    if (!opt) {
        opt = tree.put_child(*itl, func_tree{nullptr});
    }
    return put_data(std::next(itl), itr, data, *opt);
}

inline std::vector<std::string> split(const std::string &src, const std::string &delimiter) {
    if (src.empty()) {
        return {};
    }
    if (delimiter.empty()) {
        return {src};
    }
    std::vector<std::string> vRet{};
    size_t last = 0;
    size_t next = 0;
    while ((next = src.find(delimiter, last)) != std::string::npos) {
        vRet.emplace_back(src.substr(last, next - last));
        last = next + delimiter.size();
    }
    if (last < src.size()) {
        vRet.push_back(src.substr(last));
    } else if (last == src.size()) {
        vRet.push_back("");
    }
    return vRet;
}
}  // namespace tcli

#define TCLI_GEN_NAME(prefix, ...) BOOST_PP_CAT(prefix##_, LTZ_PP_CAT_WITH_SEP(XTCX, __VA_ARGS__))
#define TCLI_GEN_NAME_REG(...) TCLI_GEN_NAME(_tcli_reg, __VA_ARGS__)
#define TCLI_GEN_NAME_REG_OBJ(...) TCLI_GEN_NAME(tcli_reg_obj, __VA_ARGS__)

#define TCLIF(...)                                                                                                     \
    namespace tcli {                                                                                                   \
    struct TCLI_GEN_NAME_REG(__VA_ARGS__) : public Data {                                                              \
        TCLI_GEN_NAME_REG(__VA_ARGS__)() {                                                                             \
            std::vector<std::string> vpath = split(BOOST_PP_STRINGIZE(LTZ_PP_CAT_WITH_SEP(XTCX,__VA_ARGS__)), "XTCX"); \
            file_ = __FILE__;                                                                                          \
            line_ = __LINE__;                                                                                          \
            put_data(vpath.begin(), vpath.end(), this);                                                                \
        }                                                                                                              \
        int f(const std::vector<std::string> &tcArgs) override;                                                        \
    } TCLI_GEN_NAME_REG_OBJ(__VA_ARGS__);                                                                              \
    }                                                                                                                  \
    int tcli::TCLI_GEN_NAME_REG(__VA_ARGS__)::f(const std::vector<std::string> &tcArgs)

#endif