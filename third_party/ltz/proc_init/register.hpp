#ifndef LTZ_PROC_INIT_REGISTER_HPP
#define LTZ_PROC_INIT_REGISTER_HPP

#include <boost/property_tree/ptree.hpp>
#include "../string/string.hpp"

namespace ltz {
namespace proc_init {

namespace regi {
namespace toStrRegFlag {
const uint32_t mark_entity{1 << 0};
const uint32_t address{1 << 1};
const uint32_t default_flag{mark_entity};
}  // namespace toStrRegFlag
}  // namespace regi

template <typename T>
class reg {
   public:
    reg() = default;
    ~reg() = default;

    /* error */
   public:
    // enum class err : uint32_t {
    //     unknown,
    //     ok,
    //     node_not_found,
    // };

    // err e{err::ok};
    // std::string err_msg;

    // inline std::string toStr(err e) {
    //     return "";
    // }
    using err_t = int;
    struct error {
        static const err_t ok = 0;
        static const err_t unknown = -1;
        static const err_t node_not_found = -2;
        /* reserve to -100, derived class should start at -101 */
    };
    err_t err{error::ok};

    bool ok() {
        return err == error::ok;
    }

    inline std::string errstr() {
        switch (err) {
            case error::ok:
                return "ok";
            case error::node_not_found:
                return "node not found";
            case error::unknown:
            default:
                return "unknown";
        }
    }

   protected:
    using reg_tree = boost::property_tree::basic_ptree<std::string, T *>;
    reg_tree rt_{nullptr};
    struct to_entry_translator {
        using internal_type = T *;
        using external_type = T *;
        boost::optional<external_type> get_value(internal_type t) {
            return t;
        }
        boost::optional<internal_type> put_value(external_type t) {
            return t;
        }
    };

   public:
    /* put */
    /*
     @brief put ptr to T
     @note prefer to use this function rather than boost::property_tree:put to ensure ptr to T of path node is null
    */
    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline void put(InputIt first, InputIt last, T *entry = nullptr) {
        put_impl(first, last, entry, rt_);
    }

    inline void put(const std::string &path, T *entry = nullptr, const std::string &path_sep = "/") {
        std::vector<std::string> v = ltz::str::split(path, path_sep);
        put(v.begin(), v.end(), entry);
    }

    /* get */
    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline std::pair<T *, InputIt> get(InputIt first, InputIt last, reg_tree &tree) {
        while (first != last && *first == "") {
            first++;
        }
        if (first == last) {
            err = error::ok;
            return {tree.template get_value<T *>(to_entry_translator()), first};
        }
        auto opt = tree.get_child_optional(*first);
        if (!opt) {
            err = error::ok;
            return {tree.template get_value<T *>(to_entry_translator()), first};
        }
        return get(++first, last, *opt);
    }

    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline std::pair<T *, InputIt> get(InputIt first, InputIt last) {
        return get(first, last, rt_);
    }

    inline T *get(const std::string &path, const std::string &path_sep = "/") {
        std::vector<std::string> v = ltz::str::split(path, path_sep);
        auto pr = get(v.begin(), v.end());
        return pr.first;
    }

    /*
        @brief Get keys of children nodes, specified by path
        @param path Path to register entry, require format like /a/b/c
     */
    inline std::vector<std::string> get_children_keys(const std::string &path) {
        auto op = rt_.get_child_optional(typename reg_tree::path_type(path, '/'));
        if (!op) {
            err = error::node_not_found;
            return {};
        }
        std::vector<std::string> v;
        for (auto &key : *op) {
            v.push_back(key.first);
        }

        err = error::ok;

        return v;
    }

    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline std::vector<std::string> get_children_keys(InputIt first, InputIt last) {
        std::string path = str::join(first, last, "/");
        return get_children_keys(path);
    }

   public:
    /*
        @brief Get registered info
        @details Get string of tree and its children, format like:
            tree(this, entry addr)
            |--child_key(child_this, sub entry addr)
            |----key*(0x123456, 0xabcdef)
            ...
        @param details_flag Flag that indicate how many infomation to be show.
     */
    inline std::string toStr_registered(const std::string &tree_name = "tree", uint32_t details_flag = regi::toStrRegFlag::default_flag) {
        T *entry = rt_.template get_value<T *>();
        std::stringstream ss;
        ss << tree_name;
        if (entry && details_flag & regi::toStrRegFlag::mark_entity) {
            ss << "*";
        }
        if (details_flag & regi::toStrRegFlag::address) {
            ss << "(0x" << std::hex << (uint64_t)&rt_ << ", 0x" << (uint64_t)entry << ")";
        }
        ss << "\n";
        toStr_registered_impl(ss, rt_, details_flag, 1);
        std::string s = ss.str();
        if (s.size()) {
            s.pop_back();
        }

        err = error::ok;
        return s;
    }

    /* helper */
   private:
    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline void put_impl(InputIt first, InputIt last, T *entry, reg_tree &tree) {
        // consider path: ////ab//c//d///
        while (first != last && *first == "") {
            first++;
        }
        if (first == last) {
            auto val_opt = tree.template get_value_optional<T *>(to_entry_translator());
            if (val_opt && *val_opt) {
                throw std::runtime_error("duplicate path!");
            }
            tree.put_value(entry, to_entry_translator());

            err = error::ok;
            return;
        }
        auto opt = tree.get_child_optional(*first);
        if (!opt) {
            opt = tree.put_child(*first, reg_tree{nullptr});
        }
        put_impl(++first, last, entry, *opt);
    }

    inline std::stringstream &toStr_registered_impl(std::stringstream &ss, reg_tree &tree, uint32_t details_flag = regi::toStrRegFlag::default_flag, int depth = 0) {
        for (auto &pr : tree) {
            const std::string key_child = pr.first;
            reg_tree &tree_child = pr.second;
            T *entry_child = tree_child.template get_value<T *>();
            ss << std::string(depth * 2, ' ') << key_child;
            if (entry_child && details_flag & regi::toStrRegFlag::mark_entity) {
                ss << "*";
            }
            if (details_flag & regi::toStrRegFlag::address) {
                ss << "(0x" << std::hex << (uint64_t)&tree_child << ", 0x" << (uint64_t)entry_child << ")";
            }
            ss << "\n";
            toStr_registered_impl(ss, tree_child, details_flag, depth + 1);
        }
        return ss;
    }
};
}  // namespace proc_init
}  // namespace ltz

#endif