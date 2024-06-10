#ifndef LTZ_PROC_INIT_REGISTER_HPP
#define LTZ_PROC_INIT_REGISTER_HPP

#include <boost/property_tree/ptree.hpp>
#include "../string/string.hpp"

namespace ltz {
namespace proc_init {

template <typename T>
class reg {
   public:
    reg() = default;
    ~reg() = default;

    /* error */
   public:
    enum class err {
        ok,
        node_not_found,
    };

    inline std::string toStr(err e) {
        switch (e) {
            case err::ok:
                return "ok";
            case err::node_not_found:
                return "node not found";
            default:
                return "unknown";
        }
    }
    err e{err::ok};
    std::string err_msg{toStr(e)};

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
    bool ok() {
        return e == err::ok;
    }

    /* put */
    /*
     @brief put ptr to T
     @note prefer to use this function rather than boost::property_tree:put to ensure ptr to T of path node is null
    */
    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline void put(It itl, It itr, T *entry = nullptr) {
        put_impl(itl, itr, entry, rt_);
    }

    inline void put(const std::string &path, T *entry = nullptr, const std::string &path_sep = "/") {
        std::vector<std::string> v = ltz::str::split(path, path_sep);
        put(v.begin(), v.end(), entry);
    }

    /* get */
    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline std::pair<T *, It> get(It itl, It itr, reg_tree &tree) {
        while (itl != itr && *itl == "") {
            itl = std::next(itl);
        }
        if (itl == itr) {
            err_msg = toStr(e = err::ok);
            return {tree.template get_value<T *>(to_entry_translator()), itl};
        }
        auto opt = tree.get_child_optional(*itl);
        if (!opt) {
            err_msg = toStr(e = err::ok);
            return {tree.template get_value<T *>(to_entry_translator()), itl};
        }
        return get(std::next(itl), itr, *opt);
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline std::pair<T *, It> get(It itl, It itr) {
        return get(itl, itr, rt_);
    }

    inline T *get(const std::string &path, const std::string &path_sep = "/") {
        std::vector<std::string> v = ltz::str::split(path, path_sep);
        auto pr = get(v.begin(), v.end());
        return pr.first;
    }

    /* list */
    /*
        @brief List children nodes' key of node specified by path
        @param path Path to register entry, require format like /a/b/c
     */
    inline std::string list_children(const std::string &path) {
        auto op = rt_.get_child_optional(typename reg_tree::path_type(path, '/'));
        if (!op) {
            err_msg = toStr(e = err::node_not_found);
            return "";
        }
        std::string s;
        for (auto &key : *op) {
            s += key.first + "\n";
        }
        if (s.size()) {
            s.pop_back();
        }
        err_msg = toStr(e = err::ok);
        return s;
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline std::string list_children(It itl, It itr) {
        std::string path = str::join(itl, itr, "/");
        return list_children(path);
    }

   public:
    struct toStrRegFlag {
        static constexpr uint32_t mark_entity{1 << 0};
        static constexpr uint32_t address{1 << 1};
        static constexpr uint32_t default_flag{mark_entity};
    };
    /*
        @brief Get registered info
        @details Get string of tree and its children, format like:
            tree(this, entry addr)
            |--child_key(child_this, sub entry addr)
            |----key*(0x123456, 0xabcdef)
            ...
        @param details_flag Flag that indicate how many infomation to be show.
     */
    inline std::string toStr_registered(const std::string &tree_name = "tree", uint32_t details_flag = toStrRegFlag::default_flag) {
        T *entry = rt_.template get_value<T *>();
        std::stringstream ss;
        ss << tree_name;
        if (entry && details_flag & toStrRegFlag::mark_entity) {
            ss << "*";
        }
        if (details_flag & toStrRegFlag::address) {
            ss << "(0x" << std::hex << (uint64_t)&rt_ << ", 0x" << (uint64_t)entry << ")";
        }
        ss << "\n";
        toStr_registered_impl(ss, rt_, details_flag, 1);
        std::string s = ss.str();
        if (s.size()) {
            s.pop_back();
        }
        err_msg = toStr(e = err::ok);
        return s;
    }

    /* helper */
   private:
    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline void put_impl(It itl, It itr, T *entry, reg_tree &tree) {
        // consider path: ////ab//c//d///
        while (itl != itr && *itl == "") {
            itl = std::next(itl);
        }
        if (itl == itr) {
            auto val_opt = tree.template get_value_optional<T *>(to_entry_translator());
            if (val_opt && *val_opt) {
                throw std::runtime_error("duplicate path!");
            }
            tree.put_value(entry, to_entry_translator());
            err_msg = toStr(e = err::ok);
            return;
        }
        auto opt = tree.get_child_optional(*itl);
        if (!opt) {
            opt = tree.put_child(*itl, reg_tree{nullptr});
        }
        put_impl(std::next(itl), itr, entry, *opt);
    }

    inline std::stringstream &toStr_registered_impl(std::stringstream &ss, reg_tree &tree, uint32_t details_flag = toStrRegFlag::default_flag, int depth = 0) {
        for (auto &pr : tree) {
            const std::string key_child = pr.first;
            reg_tree &tree_child = pr.second;
            T *entry_child = tree_child.template get_value<T *>();
            ss << std::string(depth * 2, ' ') << key_child;
            if (entry_child && details_flag & toStrRegFlag::mark_entity) {
                ss << "*";
            }
            if (details_flag & toStrRegFlag::address) {
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