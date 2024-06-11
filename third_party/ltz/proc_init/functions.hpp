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

namespace fn {

struct node {
    virtual void lpif_init() {}
    virtual int lpif_main(const std::vector<std::string> &lpif_args) = 0;
    virtual void lpif_clean() {}
};

}  // namespace fn

class fn_reg : public reg<fn::node> {
   public:
    fn_reg() = default;
    ~fn_reg() = default;

    /*
        @brief Run lpif_main of node specified by iterator
        @param itl
        @param itr
        @param op Function to do something and call lpif_main and do something after call lpif_main.
            Prototype: int op(ltz::proc_init::fn::node &lpif_node, It itl, It itr, It itm);
        @todo The first parameter in op is fn::node, it cause that code in definition of op have to dynamic_cast to custom node. Find way to suport custom node as parameter of op.
     */
    template <typename It, typename Op, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline int run(It itl, It itr, Op op) {
        auto pr = get(itl, itr);
        fn::node *pNode = pr.first;
        if (!pNode) {
            e = err::node_not_found;
            err_msg = toStr(e);
            return -1;
        }
        It itm = pr.second;

        int nRet = op(*pNode, itl, itr, itm);
        e = err::ok;
        err_msg = toStr(e);
        return nRet;
    }

    template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
    inline int run(It itl, It itr) {
        return run(itl, itr, [](ltz::proc_init::fn::node &node, It &itl, It itr, It itm) -> int {
            int nRet = 0;
            node.lpif_init();
            nRet = node.lpif_main(std::vector<std::string>{itm, itr});
            node.lpif_clean();
            return nRet;
        });
    }

    inline int run(const std::string &path, const std::string &path_sep = "/") {
        std::vector<std::string> vpath = str::split(path, path_sep);
        return run(vpath.begin(), vpath.end());
    }


    /* 
        @brief todo
        @param op Function to call lpif_main()
            Prototype: int op(ltz::proc_init::fn::node& node);
     */
    inline void run_all(std::function<int(fn::node &)> op) {
        std::vector<std::string> v;
        run_all_impl(v, rt_, op);
    }

    inline void run_all() {
        return run_all([](fn::node &node) -> int {
            int nRet = 0;
            node.lpif_init();
            nRet = node.lpif_main({});
            node.lpif_clean();
            return nRet;
        });
    }

   private:
    inline void run_all_impl(std::vector<std::string> &v, reg_tree &tree, std::function<int(fn::node &)> op) {
        fn::node *pNode = tree.get_value<fn::node *>(to_entry_translator());
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

inline fn_reg &get_fn_reg(const std::string &name) {
    static std::map<std::string, fn_reg> mp_fnreg_;
    auto it = mp_fnreg_.find(name);
    if (it == mp_fnreg_.end()) {
        it = mp_fnreg_.emplace(name, fn_reg{}).first;
    }
    return it->second;
}

/*
    @brief Get node specify by iterator. If node is not exist, create it.
 */
template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
inline fn::node &get_node(const std::string &reg_name, It itl, It itr) {
    auto &fnreg = get_fn_reg(reg_name);
    auto pr = fnreg.get(itl, itr);
    fn::node *pNode = pr.first;
    if (!pNode) {
        throw std::runtime_error("get_node: node not exist");
    }
    return *pNode;
}

inline fn::node &get_node(const std::string &reg_name, const std::string &path, const std::string &path_sep = "/") {
    std::vector<std::string> vpath = ::ltz::str::split(path, path_sep);
    return get_node(reg_name, vpath.begin(), vpath.end());
}


}  // namespace fn
}  // namespace proc_init
}  // namespace ltz

#define _LTZ_PI_FN_PATH_SEP LPIFN
#define _LTZ_PI_FN_CAT2PATH(...) LTZ_PP_CAT_WITH_SEP(_LTZ_PI_FN_PATH_SEP, __VA_ARGS__)
#define _LTZ_PI_FN_GET_ID(name, path) BOOST_PP_CAT(_lpifn_##name##_, path)
#define _LTZ_PI_FN_GET_ID_VA(...) _LTZ_PI_FN_GET_ID(_LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

#define _LTZ_PI_FN_NODE_STRU(id) BOOST_PP_CAT(id, _stru)
#define _LTZ_PI_FN_NODE_STRU_OBJ(id) BOOST_PP_CAT(id, _stru_obj)

#define _LTZ_PI_FN_REG_HANDLER(handler, id) BOOST_PP_CAT(id, _handler##handler)
#define _LTZ_PI_FN_REG_HANDLER_OBJ(handler, id) BOOST_PP_CAT(id, _handler_##handler##_obj)
#define _LTZ_PI_FN_REG_HANDLE_FN(handler, id) BOOST_PP_CAT(id, _handle_fn_##handler)

#define _LTZ_PI_FN_GET_REG_IMPL(name) ::ltz::proc_init::fn::get_fn_reg(BOOST_PP_STRINGIZE(name))

// std::cout << "At " << (void *)this << " construct: " << BOOST_PP_STRINGIZE(_LTZ_PI_FN_GET_ID(name, path)) << std::endl; \

#define _LTZ_PI_FN_NODE_CONSTRUCT_IMPL(name, type, path)                                      \
    struct _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path)) : public type {                \
        _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path))() {                               \
            auto &reg = _LTZ_PI_FN_GET_REG_IMPL(name);                                        \
            reg.put(BOOST_PP_STRINGIZE(path), this, BOOST_PP_STRINGIZE(_LTZ_PI_FN_PATH_SEP)); \
        }                                                                                     \
        void lpif_init() override;                                                            \
        int lpif_main(const std::vector<std::string> &lpif_args) override;                    \
        void lpif_clean() override;                                                           \
    } _LTZ_PI_FN_NODE_STRU_OBJ(_LTZ_PI_FN_GET_ID(name, path))

#define _LTZ_PI_FN_NODE_HANDLE_IMPL(name, handle_name, path)                                                                                                                \
    void _LTZ_PI_FN_REG_HANDLE_FN(handle_name, _LTZ_PI_FN_GET_ID(name, path))(::ltz::proc_init::fn::node &);                                                                \
    struct _LTZ_PI_FN_REG_HANDLER(handle_name, _LTZ_PI_FN_GET_ID(name, path)) {                                                                                             \
        _LTZ_PI_FN_REG_HANDLER(handle_name, _LTZ_PI_FN_GET_ID(name, path))() {                                                                                              \
            ::ltz::proc_init::fn::node &node = ::ltz::proc_init::fn::get_node(BOOST_PP_STRINGIZE(name), BOOST_PP_STRINGIZE(path), BOOST_PP_STRINGIZE(_LTZ_PI_FN_PATH_SEP)); \
            _LTZ_PI_FN_REG_HANDLE_FN(handle_name, _LTZ_PI_FN_GET_ID(name, path))(node);                                                                                     \
        }                                                                                                                                                                   \
    } _LTZ_PI_FN_REG_HANDLER_OBJ(handle_name, _LTZ_PI_FN_GET_ID(name, path));                                                                                               \
    void _LTZ_PI_FN_REG_HANDLE_FN(handle_name, _LTZ_PI_FN_GET_ID(name, path))(::ltz::proc_init::fn::node & lpif_node)

#define _LTZ_PI_FN_DEF_MAIN(name, path) int _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path))::lpif_main(const std::vector<std::string> &lpif_args)
#define _LTZ_PI_FN_DEF_INIT(name, path) void _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path))::lpif_init()
#define _LTZ_PI_FN_DEF_CLEAN(name, path) void _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path))::lpif_clean()


/* ********** */
/* export */

/*
    @brief todo
    @param name Unique name that specify the register.
 */
#define LTZ_PI_FN_GET_REG(name) _LTZ_PI_FN_GET_REG_IMPL(name)

/*
    @brief Construct node
    @param name
    @param type Must derived from ltz::proc_init::fn::node
 */
#define LTZ_PI_FN_NODE_CONSTRUCT(name, type, ...) _LTZ_PI_FN_NODE_CONSTRUCT_IMPL(name, type, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

/*
    @brief todo
    @param name Unique name that specify the register.
    @param ... Variadic param to specify path
    @note Define body can use variable: lpif_args
 */
#define LTZ_PI_FN_DEF_MAIN(name, ...) _LTZ_PI_FN_DEF_MAIN(name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

#define LTZ_PI_FN_DEF_INIT(name, ...) _LTZ_PI_FN_DEF_INIT(name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))
#define LTZ_PI_FN_DEF_CLEAN(name, ...) _LTZ_PI_FN_DEF_CLEAN(name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

/*
    @brief todo
    @param name Unique name that specify the register.
    @param handle_name Unique name for register indicated by name, handle_name descibe what will be handle to node.
    @param ... Variadic param to specify path
 */
#define LTZ_PI_FN_NODE_HANDLE(name, handle_name, ...) _LTZ_PI_FN_NODE_HANDLE_IMPL(name, handle_name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

#endif