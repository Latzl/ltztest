#ifndef TCLI_GTEST_HPP
#define TCLI_GTEST_HPP

#include <tcli/tcli.hpp>
#include <tcli/opt.hpp>

#include <gtest/gtest.h>

namespace tcli {
namespace gtest {

struct node : public tcli::node {
    using gtest_t = uint32_t;
    struct type {
        static const gtest_t all = 1 << 0;
        static const gtest_t suit = 1 << 1;
        static const gtest_t case_ = 1 << 2;
    };
    // todo rename
    gtest_t t{0};

    std::string toStr_char(gtest_t t);

    std::string get_info() override;
};

int test_with_filter(const std::string& suit = "", const std::string& test = "");

}  // namespace gtest
}  // namespace tcli

// define
/*
    @brief Define a test case.
    @param ... Variadic that specify case path. Last two params indicate suit name and case name, while remaider combine to path.
 */
// todo rename to TCLI_GTEST_DEF
#define TCLI_GTEST(...) _TCLI_GTEST_I(__VA_ARGS__)

// call
/*
    @brief Start test all case. It's suppose to call at global scope if testing all case is considered.
 */
// todo rename to TCLI_GTEST_CALL_ALL
#define TCLI_GTEST_ALL() _TCLI_GTEST_ALL_I()

/*
    @brief Start test suit specified by variadic like (path, suit)
    @param ... Variadic that specify suit path. Last param indicate suit name, while remaider combine to path.
 */
#define TCLI_GTEST_SUITE(...) _TCLI_GTEST_SUIT_I(__VA_ARGS__)

/*
    @brief Start test case specified by variadic like (path, suit, case)
    @param ... Variadic that specify case path. Last two params indicate suit name and case name, while remaider combine to path.
 */
#define TCLI_GTEST_CASE(...) _TCLI_GTEST_CASE_I(__VA_ARGS__)


/* definition */
// common
#define _TCLI_FN_GTEST(...) TCLI_FN(gtest, __VA_ARGS__)
#define _TCLI_GTEST_GEN_SUIT_NAME(suit, ...) LTZ_PP_VA_CAT_WITH_UNDERLINE(__VA_ARGS__, _##suit)
#define _TCLI_GTEST_VA_GET_SUIT(...) BOOST_PP_VARIADIC_ELEM(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), __VA_ARGS__)
#define _TCLI_GTEST_VA_GET_CASE(...) BOOST_PP_VARIADIC_ELEM(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), __VA_ARGS__)
#define _TCLI_GTEST_VA_GET_PATH(...) BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FIRST_N(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

// test define
#define _TCLI_GTEST_II(suit, case, ...) TEST(_TCLI_GTEST_GEN_SUIT_NAME(suit, __VA_ARGS__), case)
#define _TCLI_GTEST_I(...) _TCLI_GTEST_II(_TCLI_GTEST_VA_GET_SUIT(__VA_ARGS__), _TCLI_GTEST_VA_GET_CASE(__VA_ARGS__), _TCLI_GTEST_VA_GET_PATH(__VA_ARGS__))

// call def
#define _TCLI_GTEST_CALL_TYPE_ALL 0
#define _TCLI_GTEST_CALL_TYPE_ALL_INST ::tcli::gtest::node::type::all
#define _TCLI_GTEST_CALL_TYPE_SUIT 1
#define _TCLI_GTEST_CALL_TYPE_SUIT_INST ::tcli::gtest::node::type::suit
#define _TCLI_GTEST_CALL_TYPE_CASE 2
#define _TCLI_GTEST_CALL_TYPE_CASE_INST ::tcli::gtest::node::type::case_

// todo to readable assign t
#define _TCLI_GTEST_CALL_DEF(type, ...)                                              \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::gtest::node, gtest, __VA_ARGS__) {        \
        t = BOOST_PP_IF(BOOST_PP_EQUAL(type, _TCLI_GTEST_CALL_TYPE_ALL), /*  */      \
            _TCLI_GTEST_CALL_TYPE_ALL_INST, /*  */                                   \
            BOOST_PP_IF(BOOST_PP_EQUAL(type, _TCLI_GTEST_CALL_TYPE_SUIT), /*  */     \
                _TCLI_GTEST_CALL_TYPE_SUIT_INST, /*  */                              \
                BOOST_PP_IF(BOOST_PP_EQUAL(type, _TCLI_GTEST_CALL_TYPE_CASE), /*  */ \
                    _TCLI_GTEST_CALL_TYPE_CASE_INST, /*  */                          \
                    BOOST_PP_EMPTY() /*  */                                          \
                    )));                                                             \
    }                                                                                \
    LTZ_PI_FN_DEF_INIT(tcli, gtest, __VA_ARGS__) {}                                  \
    LTZ_PI_FN_DEF_CLEAN(tcli, gtest, __VA_ARGS__) {}                                 \
    LTZ_PI_FN_DEF_MAIN(tcli, gtest, __VA_ARGS__)

// call all
#define _TCLI_GTEST_ALL_I()                                         \
    _TCLI_GTEST_CALL_DEF(_TCLI_GTEST_CALL_TYPE_ALL) {               \
        ::testing::InitGoogleTest(&tcli::argc_raw, tcli::argv_raw); \
        return RUN_ALL_TESTS();                                     \
    }

// call suit
#define _TCLI_GTEST_SUIT_VA_GET_SUIT(...) BOOST_PP_VARIADIC_ELEM(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), __VA_ARGS__)
#define _TCLI_GTEST_SUIT_VA_GET_PATH(...) BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FIRST_N(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))
#define _TCLI_GTEST_SUIT_II(suit, ...)                                                                           \
    _TCLI_GTEST_CALL_DEF(_TCLI_GTEST_CALL_TYPE_SUIT, __VA_ARGS__, suit) {                                        \
        return ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(_TCLI_GTEST_GEN_SUIT_NAME(suit, __VA_ARGS__))); \
    }
#define _TCLI_GTEST_SUIT_I(...) _TCLI_GTEST_SUIT_II(_TCLI_GTEST_SUIT_VA_GET_SUIT(__VA_ARGS__), _TCLI_GTEST_SUIT_VA_GET_PATH(__VA_ARGS__))

// call case
#define _TCLI_GTEST_CASE_II(suit, case, ...)                                                                                               \
    _TCLI_GTEST_CALL_DEF(_TCLI_GTEST_CALL_TYPE_CASE, __VA_ARGS__, suit, case) {                                                            \
        return ::tcli::gtest::test_with_filter(BOOST_PP_STRINGIZE(_TCLI_GTEST_GEN_SUIT_NAME(suit, __VA_ARGS__)), BOOST_PP_STRINGIZE(case)); \
    }
#define _TCLI_GTEST_CASE_I(...) _TCLI_GTEST_CASE_II(_TCLI_GTEST_VA_GET_SUIT(__VA_ARGS__), _TCLI_GTEST_VA_GET_CASE(__VA_ARGS__), _TCLI_GTEST_VA_GET_PATH(__VA_ARGS__))

#endif