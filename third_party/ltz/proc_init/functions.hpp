/*
    @file functions.hpp

    Define some macro for easy define and use function.

    Usage:
        1. Define your custom struct, called node. Custom node must derived from ltz::proc_init::fn::node.
        2. Define your macro to define function body. Here are two template
            2.1
                #define MY_DEF(...)                                             \
                    LTZ_PI_FN_NODE_CONSTRUCT(my_name, my_node, __VA_ARGS__){    \
                        // customize node constructor                           \
                    }                                                           \
                    LTZ_PI_FN_DEF_INIT(my_name, __VA_ARGS__) {                  \
                        // init function before lpif_main call                  \
                    }                                                           \
                    LTZ_PI_FN_DEF_CLEAN(my_name, __VA_ARGS__) {                 \
                        // clean funciton after lpif_main called                \
                    }                                                           \
                    LTZ_PI_FN_DEF_MAIN(my_name, __VA_ARGS__)
            2.2
                #define MY_DEF_WITH_OP(op, ...)                                     \
                    LTZ_PI_FN_NODE_CONSTRUCT(my_name, my_node, __VA_ARGS__){}       \
                    LTZ_PI_FN_NODE_HANDLE(my_name, my_handle_name, __VA_ARGS__) {   \
                        auto& node = dynamic_cast<my_node&>(lpif_node);             \
                        op(node);                                                   \
                    }                                                               \
                    LTZ_PI_FN_DEF_INIT(my_name, __VA_ARGS__) {}                     \
                    LTZ_PI_FN_DEF_CLEAN(my_name, __VA_ARGS__) {}                    \
                    LTZ_PI_FN_DEF_MAIN(my_name, __VA_ARGS__)
        3. Use your macro on global scope
            3.1
                MY_DEF(a, b, c){
                    printf("Hello world");
                    return 0;
                }
            3.2
                MY_DEF_WITH_OP([](my_node& node){ // do with node}, a , b, c){
                    printf("Hello world");
                    return 0;
                }
        4. Call fuction that your macro define.
            int main(){
                LTZ_PI_FN_GET_REG(my_name).run("a/b/c");
            }
 */
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
    using reg_p = reg<fn::node>;

    /* error */
   public:
    struct error : public reg_p::error {
        static const err_t function_invalid = -101;
    };

    inline std::string errstr() {
        switch (err) {
            case error::function_invalid:
                return "function invalid";
            default:
                return reg_p::errstr();
        }
    }
    /*
        @brief Run lpif_main of node specified by iterator
        @param first
        @param last
        @param op Function to do something and call lpif_main and do something after call lpif_main.
        @return Return value of lpif_main if run successfully. Return -1 if
            1. node not found, and set err to error::function_invalid.
            2. op is not callable, and set error to err::no_valid_function.
        @todo The first parameter in op is fn::node, it cause that code in definition of op have to dynamic_cast to custom node. Find way to suport custom node as parameter of op.
        @todo? Error handle for fn_reg
     */
    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline int run(InputIt first, InputIt last, std::function<int(fn::node &)> op) {
        if (!op) {
            err = error::function_invalid;
            return -1;
        }

        auto pr = get(first, last);
        fn::node *pNode = pr.first;
        if (!pNode) {
            err = error::function_invalid;
            return -1;
        }

        int nRet = op(*pNode);

        err = error::ok;

        return nRet;
    }

    template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
    inline int run(InputIt first, InputIt last) {
        return run(first, last, [&first, &last](fn::node &node) -> int {
            int nRet = 0;
            node.lpif_init();
            nRet = node.lpif_main(std::vector<std::string>{first, last});
            node.lpif_clean();
            return nRet;
        });
    }

    inline int run(const std::string &path, const std::string &path_sep = "/") {
        std::vector<std::string> vpath = str::split(path, path_sep);
        return run(vpath.begin(), vpath.end());
    }


    /*
        @brief Run all registered function with op().
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
template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type * = nullptr>
inline fn::node &get_node(const std::string &reg_name, InputIt first, InputIt last) {
    auto &fnreg = get_fn_reg(reg_name);
    auto pr = fnreg.get(first, last);
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
#define _LTZ_PI_FN_CAT2PATH(...) LTZ_PP_VA_CAT_WITH_SEP(_LTZ_PI_FN_PATH_SEP, __VA_ARGS__)
#define _LTZ_PI_FN_GET_ID(name, path) BOOST_PP_CAT(_lpifn_##name##_, path)
#define _LTZ_PI_FN_GET_ID_VA(...) _LTZ_PI_FN_GET_ID(_LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

#define _LTZ_PI_FN_NODE_STRU(id) BOOST_PP_CAT(id, _stru)
#define _LTZ_PI_FN_NODE_STRU_OBJ(id) BOOST_PP_CAT(id, _stru_obj)

#define _LTZ_PI_FN_REG_HANDLER(handler, id) BOOST_PP_CAT(id, _handler##handler)
#define _LTZ_PI_FN_REG_HANDLER_OBJ(handler, id) BOOST_PP_CAT(id, _handler_##handler##_obj)
#define _LTZ_PI_FN_REG_HANDLE_FN(handler, id) BOOST_PP_CAT(id, _handle_fn_##handler)

#define _LTZ_PI_FN_GET_REG_I(name) ::ltz::proc_init::fn::get_fn_reg(BOOST_PP_STRINGIZE(name))

#define _LTZ_PI_FN_NODE_CONSTRUCT_I(name, type, path)                                         \
    struct _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path)) : public type {                \
        _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path))() {                               \
            auto &reg = _LTZ_PI_FN_GET_REG_I(name);                                           \
            construct();                                                                      \
            reg.put(BOOST_PP_STRINGIZE(path), this, BOOST_PP_STRINGIZE(_LTZ_PI_FN_PATH_SEP)); \
        }                                                                                     \
        void construct();                                                                     \
        void lpif_init() override;                                                            \
        int lpif_main(const std::vector<std::string> &lpif_args) override;                    \
        void lpif_clean() override;                                                           \
    } _LTZ_PI_FN_NODE_STRU_OBJ(_LTZ_PI_FN_GET_ID(name, path));                                \
    void _LTZ_PI_FN_NODE_STRU(_LTZ_PI_FN_GET_ID(name, path))::construct()


#define _LTZ_PI_FN_NODE_HANDLE_I(name, handle_name, path)                                                                                                                   \
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

#define _LTZ_PI_FN_GET_NODE_I(name, ...)                                          \
    []() -> ::ltz::proc_init::fn::node * {                                        \
        std::vector<std::string> vPath = {LTZ_PP_VA_ENCLOSE_PARAMS(__VA_ARGS__)}; \
        return _LTZ_PI_FN_GET_REG_I(name).get(vPath.begin(), vPath.end()).first;  \
    }()

#define _LTZ_PI_FN_RUN_I(lpif_args, name, ...)                     \
    [](const std::vector<std::string> _lpif_args) -> int {         \
        auto lpif_node = _LTZ_PI_FN_GET_NODE_I(name, __VA_ARGS__); \
        if (!lpif_node) {                                          \
            return -1;                                             \
        }                                                          \
        return lpif_node->lpif_main(_lpif_args);                   \
    }(lpif_args)

/* ********** */
/* export */

/*
    @brief Get register obj by name.
    @param name Unique name that specify the register.
 */
#define LTZ_PI_FN_GET_REG(name) _LTZ_PI_FN_GET_REG_I(name)

/*
    @brief Construct node
    @param name
    @param type Must derived from ltz::proc_init::fn::node
 */
#define LTZ_PI_FN_NODE_CONSTRUCT(name, type, ...) _LTZ_PI_FN_NODE_CONSTRUCT_I(name, type, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

/*
    @brief Define main function body.
    @param name Unique name that specify the register.
    @param ... Variadic param to specify path
    @details Prototype after macro expansion:
        int <node_stru>::lpif_main(const std::vector<std::string> &lpif_args);
 */
#define LTZ_PI_FN_DEF_MAIN(name, ...) _LTZ_PI_FN_DEF_MAIN(name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

#define LTZ_PI_FN_DEF_INIT(name, ...) _LTZ_PI_FN_DEF_INIT(name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))
#define LTZ_PI_FN_DEF_CLEAN(name, ...) _LTZ_PI_FN_DEF_CLEAN(name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

/*
    @brief Define function body that handle node in register, which is specified by name.
    @param name Unique name that specify the register.
    @param handle_name Unique name for register indicated by name, handle_name descibe what is going to handle the node.
    @param ... Variadic to specify path
    @details Prototype after macro expansion:
        void <node_id>_handler_fn_<handle_name>(ltz::proc_init::fn::node & lpif_node)
 */
#define LTZ_PI_FN_NODE_HANDLE(name, handle_name, ...) _LTZ_PI_FN_NODE_HANDLE_I(name, handle_name, _LTZ_PI_FN_CAT2PATH(__VA_ARGS__))

/*
    @brief Get registered function node by name and path
 */
#define LTZ_PI_FN_GET_NODE(name, ...) _LTZ_PI_FN_GET_NODE_I(name, __VA_ARGS__)

/*
    @brief Run function
 */
#define LTZ_PI_FN_RUN(lpif_args, name, ...) _LTZ_PI_FN_RUN_I(lpif_args, name, __VA_ARGS__)

#endif