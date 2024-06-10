#ifndef LTZ_PROC_INIT_FUNCTIONS_HPP
#define LTZ_PROC_INIT_FUNCTIONS_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

#include "register.hpp"
#include <ltz/preprocessor/preprocessor.hpp>


namespace ltz {
namespace proc_init {

using fn_ic = std::function<void()>;
using fn_main = std::function<int(const std::vector<std::string> &)>;

template <typename T>
class fn_reg : public reg<T> {
   protected:
    // using typename ltz::proc_init::reg<T>::reg_tree;
    // using typename ltz::proc_init::reg<T>::err;
    // using typename ltz::proc_init::reg<T>::e;
    // using typename ltz::proc_init::reg<T>::err_msg;
    // using typename ltz::proc_init::reg<T>::toStr;
    // using typename ltz::proc_init::reg<T>::to_entry_translator;
    // using typename ltz::proc_init::reg<T>::rt_;

   public:
    fn_reg() = default;
    ~fn_reg() = default;

    /*
        @brief Run f_main of node specified by iterator
        @param itl
        @param itr
        @param op Function to do something and call f_main and do something after ccall f_main.
            Prototype: int op(const T &node, It &itl, It itr);
     */
    template <typename It, typename UnaryOp, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline int run(It itl, It itr, UnaryOp op) {
        auto pr = this->get(itl, itr);
        T *pNode = pr.first;
        if (!pNode) {
            this->e = reg<T>::err::node_not_found;
            this->err_msg = this->toStr(this->e);
            return -1;
        }
        It it = pr.second;

        int nRet = op(*pNode, it, itr);
        this->e = reg<T>::err::ok;
        this->err_msg = this->toStr(this->e);
        return nRet;
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline int run(It itl, It itr) {
        return run(itl, itr, [](const T &node, It &itl, It itr) -> int {
            int nRet = 0;
            if (node.f_init) {
                node.f_init();
            }
            nRet = node.f_main(std::vector<std::string>{itl, itr});
            if (node.f_clean) {
                node.f_clean();
            }
            return nRet;
        });
    }

    inline int run(const std::string &path, const std::string &path_sep = "/") {
        std::vector<std::string> vpath = str::split(path, path_sep);
        return run(vpath.begin(), vpath.end());
    }

    inline void run_all(std::function<int(const T &)> op) {
        std::vector<std::string> v;
        run_all_impl(v, this->rt_, op);
    }

    inline void run_all() {
        return run_all([](const T &node) -> int {
            int nRet = 0;
            if (node.f_init) {
                node.f_init();
            }
            nRet = node.f_main({});
            if (node.f_clean) {
                node.f_clean();
            }
            return nRet;
        });
    }

   private:
    inline void run_all_impl(std::vector<std::string> &v, typename reg<T>::reg_tree &tree, std::function<int(T &)> op) {
        T *pNode = tree.template get_value<T *>(typename reg<T>::to_entry_translator());
        if (pNode) {
            op(*pNode);
        }
        for (auto &pr : tree) {
            std::string key = pr.first;
            v.push_back(key);
            run_all_impl(v, pr.second, op);
            v.pop_back();
        }
    }
};

namespace fn {

struct node {
    fn_ic f_init;
    fn_main f_main;
    fn_ic f_clean;
};

template <typename T>
inline fn_reg<T> &get_fn_reg(const std::string &name) {
    static std::map<std::string, fn_reg<T>> mp_fnreg_;
    auto it = mp_fnreg_.find(name);
    if (it == mp_fnreg_.end()) {
        it = mp_fnreg_.emplace(name, fn_reg<T>{}).first;
    }
    return it->second;
}

/*
    @brief Get node specify by iterator. If node is not exist, create it.
 */
template <typename T, typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
inline T &get_create_node(const std::string &reg_name, It itl, It itr) {
    auto &fnreg = get_fn_reg<T>(reg_name);
    auto pr = fnreg.get(itl, itr);
    T *pNode = pr.first;
    if (!pNode) {
        pNode = new T{};
        fnreg.put(itl, itr, pNode);
    }
    return *pNode;
}

template <typename T>
inline T &get_create_node(const std::string &reg_name, const std::string &path, const std::string &path_sep = "/") {
    std::vector<std::string> vpath = ::ltz::str::split(path, path_sep);
    return get_create_node<T>(reg_name, vpath.begin(), vpath.end());
}


}  // namespace fn
}  // namespace proc_init
}  // namespace ltz

#define _LTZ_PI_FN_CAT2PATH_SEP LPIFN
#define _LTZ_PI_FN_CAT2PATH(...) LTZ_PP_CAT_WITH_SEP(_LTZ_PI_FN_CAT2PATH_SEP, __VA_ARGS__)
#define _LTZ_PI_FN_GET_ID(name, path) BOOST_PP_CAT(_lpifn_##name##_, path)
#define _LTZ_PI_FN_GET_ID_VA(...) _LTZ_PI_FN_GET_ID(_LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

#define _LTZ_PI_FN_REG_HANDLER(handler, id) BOOST_PP_CAT(id, _handler##handler)
#define _LTZ_PI_FN_REG_HANDLER_OBJ(handler, id) BOOST_PP_CAT(id, _handler_##handler##_obj)
#define _LTZ_PI_FN_REG_HANDLE_FN(handler, id) BOOST_PP_CAT(id, _handle_fn_##handler)

#define _LTZ_PI_FN_MAIN_FN(id) BOOST_PP_CAT(id, _main_fn)

#define _LTZ_PI_FN_GET_REG_IMPL(name, type) ::ltz::proc_init::fn::get_fn_reg<type>(BOOST_PP_STRINGIZE(name))

#define _LTZ_PI_FN_NODE_HANDLE_IMPL(name, type, handle_name, path)                                                                                                     \
    void _LTZ_PI_FN_REG_HANDLE_FN(handle_name, _LTZ_PI_FN_GET_ID(name, path))(type &);                                                                                 \
    struct _LTZ_PI_FN_REG_HANDLER(handle_name, _LTZ_PI_FN_GET_ID(name, path)) {                                                                                        \
        _LTZ_PI_FN_REG_HANDLER(handle_name, _LTZ_PI_FN_GET_ID(name, path))() {                                                                                         \
            type &node = ::ltz::proc_init::fn::get_create_node<type>(BOOST_PP_STRINGIZE(name), BOOST_PP_STRINGIZE(path), BOOST_PP_STRINGIZE(_LTZ_PI_FN_CAT2PATH_SEP)); \
            _LTZ_PI_FN_REG_HANDLE_FN(handle_name, _LTZ_PI_FN_GET_ID(name, path))(node);                                                                                \
        }                                                                                                                                                              \
    } _LTZ_PI_FN_REG_HANDLER_OBJ(handle_name, _LTZ_PI_FN_GET_ID(name, path));                                                                                          \
    void _LTZ_PI_FN_REG_HANDLE_FN(handle_name, _LTZ_PI_FN_GET_ID(name, path))(type & lpif_node)

#define _LTZ_PI_FN_SET_MAIN(name, type, path)                                                \
    int _LTZ_PI_FN_MAIN_FN(_LTZ_PI_FN_GET_ID(name, path))(const std::vector<std::string> &); \
    _LTZ_PI_FN_NODE_HANDLE_IMPL(name, type, set_main, path) {                                \
        lpif_node.f_main = _LTZ_PI_FN_MAIN_FN(_LTZ_PI_FN_GET_ID(name, path));                \
    }                                                                                        \
    int _LTZ_PI_FN_MAIN_FN(_LTZ_PI_FN_GET_ID(name, path))(const std::vector<std::string> &lpif_args)


/* ********** */
/* export */

/*
    @brief todo
    @param name Unique name that specify the register.
    @param type Type that to be register
 */
#define LTZ_PI_FN_GET_REG(name, type) _LTZ_PI_FN_GET_REG_IMPL(name, type)

/*
    @brief todo
    @param name Unique name that specify the register.
    @param type Type that to be register.
    @param ... Variadic param to specify path
 */
#define LTZ_PI_FN_SET_MAIN(name, type, ...) _LTZ_PI_FN_SET_MAIN(name, type, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

/*
    @brief todo
    @param name Unique name that specify the register.
    @param type Type that to be register.
    @param handle_name Unique name for register indicated by name, handle_name descibe what will be handle to node.
    @param ... Variadic param to specify path
 */
#define LTZ_PI_FN_NODE_HANDLE(name, type, handle_name, ...) _LTZ_PI_FN_NODE_HANDLE_IMPL(name, type, handle_name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

#endif