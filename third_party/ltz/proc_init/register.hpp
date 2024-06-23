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
    using err_t = int;
    struct error {
        static const err_t ok = 0;
        static const err_t unknown = -1;
        static const err_t node_not_found = -2;
        static const err_t duplicate_path = -3;
        static const err_t bad_access_node = -4;
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
            case error::duplicate_path:
                return "duplicate path";
            case error::bad_access_node:
                return "bad access node";
            case error::unknown:
            default:
                return "unknown";
        }
    }

    /* path type */
   public:
    struct path_type {
        path_type(const std::string &path, const std::string &path_sep = "/") : vec(str::split_if(path, path_sep, [](const std::string &s) { return !s.empty(); })) {}
        template <typename Str>
        path_type(Str path, Str path_sep = "/") : path_type(std::string(path), std::string(path_sep)) {}

        std::vector<std::string> vec;

        bool empty() const {
            return vec.empty();
        }
    };

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
    inline void put(InputIt first, InputIt last, T *entry) {
        if (!entry) {
            err = error::bad_access_node;
            return;
        }
        put_impl(first, last, entry, rt_);
    }

    inline void put(const std::string &path, T *entry, const std::string &path_sep = "/") {
        std::vector<std::string> v = ltz::str::split(path, path_sep);
        put(v.begin(), v.end(), entry);
    }

    /* get */
   protected:
    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline std::pair<reg_tree &, InputIt> get_tree_node(InputIt first, InputIt last, reg_tree &tree) {
        while (first != last && *first == "") {
            first++;
        }
        if (first == last) {
            err = error::ok;
            return {tree, last};
        }
        auto opt = tree.get_child_optional(*first);
        if (!opt) {
            err = error::ok;
            return {tree, first};
        }
        return get_tree_node(++first, last, *opt);
    }

   public:
    /*
        @return A pair of T * and it, it is point to the position pass the found node path
    */
    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline std::pair<T *, InputIt> get(InputIt first, InputIt last) {
        auto pr = get_tree_node(first, last, rt_);
        auto &tree_node = pr.first;
        auto it = pr.second;

        return {tree_node.template get_value<T *>(to_entry_translator()), it};
    }

    inline T *get(const std::string &path, const std::string &path_sep = "/") {
        std::vector<std::string> v = ltz::str::split(path, path_sep);
        auto pr = get(v.begin(), v.end());
        return pr.first;
    }


    /* traversal */
    struct for_each_ctx {
        std::string node_name;
        int depth;
    };
    /*
        @brief for each, in preoder traversal
        @param[in] fn prototype:
            void fn(T &);
            void fn(T &, const for_each_ctx &);
        @param[in] depth -1 means all depth, traversal on current node children will stop if depth == 0
     */
    template <typename Fn>
    inline void for_each(Fn fn, int depth = -1) {
        for_each_impl(fn, 0, depth, rt_);
        err = error::ok;
    }

    template <typename Fn>
    inline void for_each_at(Fn fn, const path_type &path, int depth = -1) {
        auto pr = get_tree_node(path.vec.begin(), path.vec.end(), rt_);
        if (pr.second == path.vec.end()) {
            err = error::node_not_found;
            return;
        }
        auto &tree_node = pr.first;
        for_each_impl(fn, 0, depth, tree_node);
        err = error::ok;
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
                // throw std::runtime_error("duplicate path!");
                err = error::duplicate_path;
                return;
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

    inline void for_each_impl(std::function<void(T &)> fn, int depth, int max_depth, reg_tree &tree) {
        if (max_depth != -1 && depth == max_depth) {
            return;
        }
        for (auto &pr : tree) {
            reg_tree &tree_child = pr.second;
            T *entry_child = tree_child.template get_value<T *>();
            if (entry_child && fn) {
                fn(*entry_child);
            }
            for_each_impl(fn, depth + 1, max_depth, tree_child);
        }
    }

    inline void for_each_impl(std::function<void(T &, const for_each_ctx &)> fn, int depth, int max_depth, reg_tree &tree) {
        if (max_depth != -1 && depth == max_depth) {
            return;
        }
        for (auto &pr : tree) {
            reg_tree &tree_child = pr.second;
            T *entry_child = tree_child.template get_value<T *>();
            if (fn) {
                const std::string key_child = pr.first;
                for_each_ctx ctx{key_child, depth};
                fn(*entry_child, ctx);
            }
            for_each_impl(fn, depth + 1, max_depth, tree_child);
        }
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