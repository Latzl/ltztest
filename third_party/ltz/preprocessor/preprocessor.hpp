#ifndef LTZ_PREPROCESSOR_HPP
#define LTZ_PREPROCESSOR_HPP

#include <boost/preprocessor.hpp>

#define _LTZ_PP_JOIN_SEP(i)                         \
    BOOST_PP_APPLY(BOOST_PP_TUPLE_ELEM(5, i, /*  */ \
        (BOOST_PP_NIL, /*  */                       \
            ("+"), /*  */                           \
            ("-"), /*  */                           \
            ("*"), /*  */                           \
            ("/"), /*  */                           \
            )))

#define LTZ_PP_JOIN_SEP_NIL 0
#define LTZ_PP_JOIN_SEP_ADD 1
#define LTZ_PP_JOIN_SEP_SUB 2
#define LTZ_PP_JOIN_SEP_MUL 3
#define LTZ_PP_JOIN_SEP_DIV 4

#define _LTZ_PP_JOIN_I_FE_OP(s, data, elem) _LTZ_PP_JOIN_SEP(data) BOOST_PP_STRINGIZE(elem)
#define _LTZ_PP_JOIN_I(sep_idx, ...) BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(0, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))) BOOST_PP_SEQ_FOR_EACH(_LTZ_PP_JOIN_I_FE_OP, sep_idx, BOOST_PP_SEQ_TAIL(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define _LTZ_PP_CAT_WITH_SEP_I_OP(s, sep, elem) BOOST_PP_CAT(sep, elem)
#define _LTZ_PP_CAT_WITH_SEP_I(sep, ...) BOOST_PP_SEQ_CAT(BOOST_PP_SEQ_PUSH_FRONT(BOOST_PP_SEQ_POP_FRONT(BOOST_PP_SEQ_TRANSFORM(_LTZ_PP_CAT_WITH_SEP_I_OP, sep, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))), BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)))

#define _LTZ_PP_ENCLOSE_ELEMS_I_OP(s, data, elem) BOOST_PP_STRINGIZE(elem)
#define _LTZ_PP_ENCLOSE_ELEMS_I(...) BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(_LTZ_PP_ENCLOSE_ELEMS_I_OP, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

/* rotate shift */
#define _LTZ_PP_ROR_I_SHIFT_CNT1(n, size) BOOST_PP_MOD(n, size)
#define _LTZ_PP_ROR_I_SHIFT_CNT(n, list) _LTZ_PP_ROR_I_SHIFT_CNT1(n, BOOST_PP_LIST_SIZE(list))
#define _LTZ_PP_ROR_I_FIRST(n, list) BOOST_PP_LIST_FIRST_N(BOOST_PP_SUB(BOOST_PP_LIST_SIZE(list), _LTZ_PP_ROR_I_SHIFT_CNT(n, list)), list)
#define _LTZ_PP_ROR_I_REST(n, list) BOOST_PP_LIST_REST_N(BOOST_PP_SUB(BOOST_PP_LIST_SIZE(list), _LTZ_PP_ROR_I_SHIFT_CNT(n, list)), list)
#define _LTZ_PP_ROR_I_SHIFTED(n, list) BOOST_PP_LIST_APPEND(_LTZ_PP_ROR_I_REST(n, list), _LTZ_PP_ROR_I_FIRST(n, list))

#define _LTZ_PP_ROR_I(n, ...) BOOST_PP_LIST_ENUM(_LTZ_PP_ROR_I_SHIFTED(n, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)))
#define _LTZ_PP_ROL_I(n, ...) _LTZ_PP_ROR_I(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), _LTZ_PP_ROR_I_SHIFT_CNT1(n, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))), __VA_ARGS__)

/* export */

/* 
    @brief Concatenate each parameter with sep to string.
    @param sep Separator, must one of LTZ_PP_JOIN_SEP_(NIL|ADD|SUB|MUL|DIV)
    @param ... Variadic to concatenate
    @return Concatenated string
 */
#define LTZ_PP_JOIN(sep, ...) _LTZ_PP_JOIN_I(sep, __VA_ARGS__)

/* 
    @brief Concatenate each parameter with sep.
    @param sep Separator
    @param ... Variadic to concatenate
    @return Variadic concatenated
 */
#define LTZ_PP_CAT_WITH_SEP(sep, ...) _LTZ_PP_CAT_WITH_SEP_I(sep, __VA_ARGS__)

#define LTZ_PP_CAT_WITH_UNDERLINE(...) LTZ_PP_CAT_WITH_SEP(_, __VA_ARGS__)

/* 
    @brief Enclose each parameter in variadic to string.
    @param ... Variadic to enclose
    @return Variadic enclosed
 */
#define LTZ_PP_ENCLOSE_ELEMS(...) _LTZ_PP_ENCLOSE_ELEMS_I(__VA_ARGS__)

/* 
    @brief Rotate right n bits
    @param n Number of bits to rotate, n must >= 0. 
    @param ... Variadic to rotate
    @return Variadict rotated
 */
#define LTZ_PP_ROR(n, ...) _LTZ_PP_ROR_I(n, __VA_ARGS__)

/* 
    @brief Rotate left n bits
 */
#define LTZ_PP_ROL(n, ...) _LTZ_PP_ROL_I(n, __VA_ARGS__)

#endif