/*
    @brief ltz process initialization utility
    @details Do something before enter main. Such as instantiate object, register some function or information
 */
#ifndef _ltz_proc_init_hpp_
#define _ltz_proc_init_hpp_

#include <map>
#include <string>
#include <memory>
#include <boost/property_tree/ptree.hpp>

namespace ltz {
namespace proc_init {

template <class T>
class Register;

template <class T>
class Register {
    using reg_tree = boost::property_tree::basic_ptree<std::string, T>;

   public:
    Register() = default;

    inline Register<T> &put(const std::string &path, const T &value) {
        if (rt_.get_child_optional(path)) {
            throw std::runtime_error("path already exist");
        }
        rt_.put(path, value);
        return *this;
    }

    inline T &get(const std::string &path) {
        return rt_.get_child(path).data();
    }

    inline boost::optional<T &> get_optional(const std::string &path) {
        auto op = rt_.get_child_optional(path);
        if(op){
            return op->data();
        }
        return boost::none;
    }

    inline std::string list_children(const std::string &path){
        auto op = rt_.get_child_optional(path);
        if(!op){
            return "";
        }
        std::string s;
        for(auto &key : *op){
            s += key.first + "\n";
        }
        if(s.size()){
            s.pop_back();
        }
        return s;
    }

   public:
    reg_tree rt_;
};

}  // namespace proc_init
}  // namespace ltz


#endif