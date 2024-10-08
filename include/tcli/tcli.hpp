/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#ifndef TCLI_HPP
#define TCLI_HPP

#include <string>
#include <vector>
#include <ltz/proc_init/proc_init.hpp>

namespace tcli {

extern int argc_raw;
extern char** argv_raw;
extern std::vector<std::string> args_pass2fn, args_fn_path;

struct basic_node : public ltz::proc_init::fn::node {
    /*
        @brief Get infomation string
        @return Infomation string. Format should conform:
            <a>{<b>[, <c>[, <d>...]]}
            where
                a: name to descipt this class
                b: info
                c: info1
                d: info2
                ...
     */
    virtual std::string get_info() {
        return "";
    };
};

struct node : public basic_node {
    std::string tcli_desc;
    std::string get_info() override;
};

int main(int argc, char* argv[]);

ltz::proc_init::fn_reg& get_register();

}  // namespace tcli


/*
    @brief Define function.
    @param ... Variable parameter, which specify the function path.
 */
#define TCLI_FN(...)                                             \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::node, __VA_ARGS__) {} \
    LTZ_PI_FN_DEF_INIT(tcli, __VA_ARGS__) {}                     \
    LTZ_PI_FN_DEF_CLEAN(tcli, __VA_ARGS__) {}                    \
    LTZ_PI_FN_DEF_MAIN(tcli, __VA_ARGS__)

/*
    @brief Define fuction with descriptrion
    @param description Function description
    @param ... Variable parameter, which specify the function path.
 */
#define TCLI_DF(description, ...)                               \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::node, __VA_ARGS__) { \
        tcli_desc = description;                                \
    }                                                           \
    LTZ_PI_FN_DEF_INIT(tcli, __VA_ARGS__) {}                    \
    LTZ_PI_FN_DEF_CLEAN(tcli, __VA_ARGS__) {}                   \
    LTZ_PI_FN_DEF_MAIN(tcli, __VA_ARGS__)

/*
    @brief Run tcli function registed.
 */
#define TCLI_RF(lpif_args, ...) LTZ_PI_FN_RUN(lpif_args, tcli, __VA_ARGS__)
#endif
