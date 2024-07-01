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

// define
/*
    @brief Define a test case.
    @param ... Variadic that specify case path. Last two params indicate suit name and case name, while remaider combine to path.
 */
#define TCLI_GTEST_DEF(...) _TCLI_GTEST_DEF_I(__VA_ARGS__)

// call
/*
    @brief Start test all case. It's suppose to call at global scope if testing all case is considered.
 */
#define TCLI_GTEST_CALL_ALL() _TCLI_GTEST_CALL_ALL_I()

/*
    @brief Start test suit specified by variadic like (path, suit)
    @param ... Variadic that specify suit path. Last param indicate suit name, while remaider combine to path.
 */
#define TCLI_GTEST_CALL_SUITE(...) _TCLI_GTEST_CALL_SUIT_I(__VA_ARGS__)

/*
    @brief Start test case specified by variadic like (path, suit, case)
    @param ... Variadic that specify case path. Last two params indicate suit name and case name, while remaider combine to path.
 */
#define TCLI_GTEST_CALL_CASE(...) _TCLI_GTEST_CALL_CASE_I(__VA_ARGS__)

/*
    @brief Start test suits under specify path
    @param ... Variadic that specify node path, suits under this node will start test.
 */
#define TCLI_GTEST_CALL_SUBS(...) _TCLI_GTEST_CALL_SUBS_I(__VA_ARGS__)


/* definition */
// common
#define _TCLI_GTEST_CALL_TYPE_CASE 0
#define _TCLI_GTEST_CALL_TYPE_CASE_INST ::tcli::gtest::node::test_type::case_

#define _TCLI_FN_GTEST(...) TCLI_FN(gtest, __VA_ARGS__)
#define _TCLI_GTEST_GEN_SUIT_NAME(suit, ...) LTZ_PP_VA_CAT_WITH_UNDERLINE(__VA_ARGS__, _##suit)
#define _TCLI_GTEST_VA_GET_SUIT(...) BOOST_PP_VARIADIC_ELEM(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), __VA_ARGS__)
#define _TCLI_GTEST_VA_GET_CASE(...) BOOST_PP_VARIADIC_ELEM(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), __VA_ARGS__)
#define _TCLI_GTEST_VA_GET_PATH(...) BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FIRST_N(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))


// test define
#define _TCLI_GTEST_DEF_II(suit, case, ...)                                               \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::gtest::node, gtest, __VA_ARGS__, suit, case) { \
        type = _TCLI_GTEST_CALL_TYPE_CASE_INST;                                                \
    }                                                                                     \
    LTZ_PI_FN_DEF_INIT(tcli, gtest, __VA_ARGS__, suit, case) {}                           \
    LTZ_PI_FN_DEF_CLEAN(tcli, gtest, __VA_ARGS__, suit, case) {}                          \
    LTZ_PI_FN_DEF_MAIN(tcli, gtest, __VA_ARGS__, suit, case) {}                           \
    TEST(_TCLI_GTEST_GEN_SUIT_NAME(suit, __VA_ARGS__), case)
#define _TCLI_GTEST_DEF_I(...) _TCLI_GTEST_DEF_II(_TCLI_GTEST_VA_GET_SUIT(__VA_ARGS__), _TCLI_GTEST_VA_GET_CASE(__VA_ARGS__), _TCLI_GTEST_VA_GET_PATH(__VA_ARGS__))

#endif