#ifndef TCLI_GTEST_HPP
#define TCLI_GTEST_HPP

#include <tcli/tcli.hpp>

#include <gtest/gtest.h>

namespace tcli {
namespace gtest {

struct node : public basic_node {
    using type_t = uint32_t;
    struct test_type {
        static const type_t case_ = 1;
    };
    type_t type{0};

    std::string toStr_char(type_t t);

    std::string get_info() override;
};

}  // namespace gtest
}  // namespace tcli

// declare
/*
    @brief Define a test case.
    @param ... Variadic that specify case path. Last two params indicate suit name and case name, while remaider combine
   to path.
    @note For test defined case, specify path to case when tcli.
        For test suit, specify path to suit when tcli.
        For test suit recursively, specify path not point to suit node.
        All path should start with 'gtest' as first node.
        For example:
            TCLI_GTEST_FN(a, b, c)
            TCLI_GTEST_FN(a, b, d)
            TCLI_GTEST_FN(a, e, f)
            TCLI_GTEST_FN(g, h)
            `tcli gtest` will test all four case.
            `tcli gtest a b c` will test case c
            `tcli gtest a b` will test suit a.
            `tcli gtest a` will test case under a.
 */
#define TCLI_GTEST_FN(...) _TCLI_GTEST_FN_I(__VA_ARGS__)

/* definition */
// common
#define _TCLI_GTEST_CALL_TYPE_CASE 0
#define _TCLI_GTEST_CALL_TYPE_CASE_INST ::tcli::gtest::node::test_type::case_

#define _TCLI_GTEST_GEN_SUIT_NAME(suit, ...) LTZ_PP_VA_CAT_WITH_UNDERLINE(__VA_ARGS__, suit)
#define _TCLI_GTEST_VA_GET_SUIT(...) \
    BOOST_PP_VARIADIC_ELEM(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), __VA_ARGS__)
#define _TCLI_GTEST_VA_GET_CASE(...) \
    BOOST_PP_VARIADIC_ELEM(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), __VA_ARGS__)
#define _TCLI_GTEST_VA_GET_PATH(...)        \
    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FIRST_N( \
        BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))


// test define
#define _TCLI_GTEST_FN_III(suit, case, ...)                                        \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::gtest::node, __VA_ARGS__, suit, case) { \
        type = _TCLI_GTEST_CALL_TYPE_CASE_INST;                                    \
    }                                                                              \
    LTZ_PI_FN_DEF_INIT(tcli, __VA_ARGS__, suit, case) {}                           \
    LTZ_PI_FN_DEF_CLEAN(tcli, __VA_ARGS__, suit, case) {}                          \
    LTZ_PI_FN_DEF_MAIN(tcli, __VA_ARGS__, suit, case) {                            \
        return 0;                                                                  \
    }                                                                              \
    TEST(_TCLI_GTEST_GEN_SUIT_NAME(suit, __VA_ARGS__), case)

#define _TCLI_GTEST_FN_II(...)                                                                     \
    _TCLI_GTEST_FN_III(_TCLI_GTEST_VA_GET_SUIT(__VA_ARGS__), _TCLI_GTEST_VA_GET_CASE(__VA_ARGS__), \
        _TCLI_GTEST_VA_GET_PATH(__VA_ARGS__))

#define _TCLI_GTEST_FN_I(...) _TCLI_GTEST_FN_II(gtest, __VA_ARGS__)

#endif