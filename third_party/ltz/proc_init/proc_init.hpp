/*
    @brief ltz process initialization utility
    @details Do something before enter main. Such as instantiate object, register some function or information
 */
#ifndef LTZ_PROC_INIT_HPP
#define LTZ_PROC_INIT_HPP

#include <map>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include "../preprocessor/preprocessor.hpp"

namespace ltz {
namespace proc_init {

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

inline std::string join(const std::vector<std::string> &v, const std::string &delimiter) {
    std::string s;
    for (size_t i = 0; i < v.size(); i++) {
        s += v[i] + delimiter;
    }
    if (v.size() && s.size()) {
        s.erase(s.size() - delimiter.size());
    }
    return s;
}

struct Data {
    virtual int f(const std::vector<std::string> &lpiArgs) = 0;
    std::string desc_;
    std::string file_;
    int line_{0};
};

class Register {
   public:
    Register(const std::string &s) : name_(s){};
    ~Register() = default;

    const std::string name_;
    bool silence_{false};
    bool ok_{true};

   private:
    using reg_tree = boost::property_tree::basic_ptree<std::string, Data *>;
    reg_tree rt_;
    struct to_data_translator {
        using internal_type = Data *;
        using external_type = Data *;
        boost::optional<external_type> get_value(internal_type t) {
            return t;
        }
        boost::optional<internal_type> put_value(external_type t) {
            return t;
        }
    };

   public:
    /*
     @brief put ptr to data
     @note prefer to use this function rather than boost::property_tree:put to ensure ptr to data of path node is null
    */
    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline void put(It itl, It itr, Data *data = nullptr) {
        put_impl(itl, itr, data, rt_);
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline int run(It itl, It itr) {
        ok_ = true;
        auto pr = get(itl, itr, rt_);
        if (!pr.first) {
            std::cerr << "Invalid path: " << path2str(itl, itr) << std::endl;
            ok_ = false;
            return -1;
        }

        Timer timer;
        if (!silence_) {
            std::stringstream ss;
            ss << "======== " << path2str(itl, pr.second);
            if (pr.second != itr) {
                ss << " " << join(std::vector<std::string>(pr.second, itr), " ");
            }
            ss << " ========";
            std::cout << ss.str() << std::endl;
        }
        int r = pr.first->f(std::vector<std::string>(pr.second, itr));
        auto pr_timer = timer.elapsed();

        if (!silence_) {
            std::cout << "======== "
                      << "return value: " << r << ", time cost: " << std::fixed << std::setprecision(2) << pr_timer.first << Timer::toStr(pr_timer.second)  //
                      << " ========" << std::endl;
        }
        return r;
    }

    inline int run(const std::string &path) {
        std::vector<std::string> v = split(path, "/");
        return run(v.begin(), v.end());
    }

    inline void run_all() {
        ok_ = true;
        std::vector<std::string> v;
        for (auto &pr : rt_) {
            std::string key = pr.first;
            v.push_back(key);
            run_all_impl(v, pr.second);
            v.pop_back();
        }
    }

    inline std::string list_children(const std::string &path) {
        ok_ = true;
        auto op = rt_.get_child_optional(reg_tree::path_type(path, '/'));
        if (!op) {
            ok_ = false;
            return "";
        }
        std::string s;
        for (auto &key : *op) {
            s += key.first + "\n";
        }
        if (s.size()) {
            s.pop_back();
        }
        return s;
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline std::string list_children(It itl, It itr) {
        std::string path = join(std::vector<std::string>(itl, itr), "/");
        return list_children(path);
    }

    /*
        @brief get registered info
        @details get string of tree and its children, format like:
            tree(this, ptr to data)
            |--child_key(child_this, ptr to child_data)
            |----key(0x123456, 0xabcdef)
            ...
        @note use put_data to ensure ptr to data of path node is null
     */
    inline std::string toStr_registered(int level = 0, const std::string &tree_name = "tree") {
        ok_ = true;
        Data *data = rt_.get_value<Data *>();
        std::stringstream ss;
        ss << tree_name;
        if (level > 0) {
            ss << "(0x" << std::hex << (long int)&rt_ << ", 0x" << (long int)data << ")";
        }
        ss << "\n";
        toStr_registered_impl(ss, rt_, level, 1);
        std::string s = ss.str();
        if (s.size()) {
            s.pop_back();
        }
        return s;
    }

    /* helper */
   private:
    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline static std::string path2str(It itl, It itr) {
        std::stringstream ss;
        while (itl != itr) {
            ss << *itl << "/";
            itl = std::next(itl);
        }
        std::string s = ss.str();
        if (s.size()) {
            s.pop_back();
        }
        return s;
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline static void put_impl(It itl, It itr, Data *data, reg_tree &tree) {
        if (itl > itr) {
            return;
        }
        if (itl == itr) {
            tree.put_value(data, to_data_translator());
            return;
        }
        auto opt = tree.get_child_optional(*itl);
        if (!opt) {
            opt = tree.put_child(*itl, reg_tree{nullptr});
        }
        put_impl(std::next(itl), itr, data, *opt);
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline std::pair<Data *, It> get(It itl, It itr, reg_tree &tree) {
        if (itl > itr) {
            return {nullptr, itr};
        }
        if (itl == itr) {
            return {tree.get_value<Data *>(to_data_translator()), itl};
        }
        if (*itl == "--") {
            return {tree.get_value<Data *>(to_data_translator()), std::next(itl)};
        }
        auto opt = tree.get_child_optional(*itl);
        if (!opt) {
            return {tree.get_value<Data *>(to_data_translator()), itl};
        }
        return get(std::next(itl), itr, *opt);
    }

    inline void run_all_impl(std::vector<std::string> &v, reg_tree &tree) {
        Data *data = tree.get_value<Data *>(to_data_translator());
        if (data) {
            Timer timer;
            if (!silence_) {
                std::cout << "======== " << path2str(v.begin(), v.end()) << " ========" << std::endl;
            }
            int r = data->f({});
            auto pr_timer = timer.elapsed();
            if (!silence_) {
                std::cout << "======== "
                          << "return value: " << r << ", time cost: " << std::fixed << std::setprecision(2) << pr_timer.first << Timer::toStr(pr_timer.second)  //
                          << " ========" << std::endl;
            }
        }
        for (auto &pr : tree) {
            std::string key = pr.first;
            v.push_back(key);
            run_all_impl(v, pr.second);
            v.pop_back();
        }
    }

    inline std::stringstream &toStr_registered_impl(std::stringstream &ss, reg_tree &tree, int level = 0, int depth = 0) {
        for (auto &pr : tree) {
            const std::string key_child = pr.first;
            reg_tree &tree_child = pr.second;
            Data *data_child = tree_child.get_value<Data *>();
            ss << std::string(depth * 2, ' ') << key_child;
            if (level > 0) {
                ss << "(0x" << std::hex << (long int)&tree_child << ", 0x" << (long int)data_child << ")";
            }
            ss << "\n";
            toStr_registered_impl(ss, tree_child, level, depth + 1);
        }
        return ss;
    }

    struct Timer {
        std::chrono::time_point<std::chrono::steady_clock> start;
        Timer() : start(std::chrono::steady_clock::now()) {}
        enum class Unit { ns, us, ms, s };
        std::pair<double, Unit> elapsed() const {
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            double ns = duration.count();
            Unit unit;
            if (ns < 1e3) {
                unit = Unit::ns;
            } else if (ns < 1e6) {
                ns /= 1e3;
                unit = Unit::us;
            } else if (ns < 1e9) {
                ns /= 1e6;
                unit = Unit::ms;
            } else {
                ns /= 1e9;
                unit = Unit::s;
            }
            return {ns, unit};
        }
        static std::string toStr(Unit unit) {
            switch (unit) {
                case Unit::ns:
                    return " ns";
                case Unit::us:
                    return " us";
                case Unit::ms:
                    return " ms";
                case Unit::s:
                    return " s";
                default:
                    assert(false);
            }
        }
    };
};

inline Register &get_register(const std::string &name) {
    static std::map<std::string, std::unique_ptr<Register>> registers;
    auto it = registers.find(name);
    if (it == registers.end()) {
        it = registers.emplace(name, std::unique_ptr<Register>(new Register(name))).first;
    }
    return *(it->second);
}

}  // namespace proc_init
}  // namespace ltz

#define LTZ_PI_CAT(...) LTZ_PP_CAT_WITH_SEP(XLPIX, __VA_ARGS__)
#define LTZ_PI_GEN_NAME(prefix, ...) BOOST_PP_CAT(prefix##_, LTZ_PI_CAT(__VA_ARGS__))
#define LTZ_PI_GEN_NAME_REG(name, ...) LTZ_PI_GEN_NAME(_lpi_reg_##name, __VA_ARGS__)
#define LTZ_PI_GEN_NAME_REG_OBJ(name, ...) LTZ_PI_GEN_NAME(lpi_reg_obj_##name, __VA_ARGS__)

#define LTZ_PI_F(name, ...)                                                                                               \
    struct LTZ_PI_GEN_NAME_REG(name, __VA_ARGS__) : public ltz::proc_init::Data {                                         \
        LTZ_PI_GEN_NAME_REG(name, __VA_ARGS__)() {                                                                        \
            std::vector<std::string> vpath = ltz::proc_init::split(BOOST_PP_STRINGIZE(LTZ_PI_CAT(__VA_ARGS__)), "XLPIX"); \
            file_ = __FILE__;                                                                                             \
            line_ = __LINE__;                                                                                             \
            auto &reg = ltz::proc_init::get_register(BOOST_PP_STRINGIZE(name));                                           \
            reg.put(vpath.begin(), vpath.end(), this);                                                                    \
        }                                                                                                                 \
        int f(const std::vector<std::string> &lpiArgs) override;                                                          \
    } LTZ_PI_GEN_NAME_REG_OBJ(name, __VA_ARGS__);                                                                         \
    int LTZ_PI_GEN_NAME_REG(name, __VA_ARGS__)::f(const std::vector<std::string> &lpiArgs)

#endif