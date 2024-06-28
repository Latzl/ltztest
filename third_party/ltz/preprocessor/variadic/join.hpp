#ifndef LTZ_PREPROCESSOR_VARIADIC_JOIN_HPP
#define LTZ_PREPROCESSOR_VARIADIC_JOIN_HPP

#include <boost/preprocessor.hpp>

#define LTZ_PP_VA_JOIN_SEP_NIL 0
#define LTZ_PP_VA_JOIN_SEP_ADD 1
#define LTZ_PP_VA_JOIN_SEP_SUB 2
#define LTZ_PP_VA_JOIN_SEP_MUL 3
#define LTZ_PP_VA_JOIN_SEP_DIV 4

/*
    @brief Concatenate each parameter with sep to string.
    @param sep Separator, must one of LTZ_PP_VA_JOIN_SEP_(NIL|ADD|SUB|MUL|DIV)
    @param ... Variadic to concatenate
    @return Concatenated string
 */
#define LTZ_PP_VA_JOIN(sep, ...) _LTZ_PP_VA_JOIN_I(sep, __VA_ARGS__)


/* definition */
#define _LTZ_PP_VA_JOIN_SEP(i)                      \
    BOOST_PP_APPLY(BOOST_PP_TUPLE_ELEM(5, i, /*  */ \
        (BOOST_PP_NIL, /*  */                       \
            ("+"), /*  */                           \
            ("-"), /*  */                           \
            ("*"), /*  */                           \
            ("/"), /*  */                           \
            )))
#define _LTZ_PP_VA_JOIN_I_FE_OP(s, data, elem) _LTZ_PP_VA_JOIN_SEP(data) BOOST_PP_STRINGIZE(elem)
#define _LTZ_PP_VA_JOIN_I(sep_idx, ...) BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(0, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))) BOOST_PP_SEQ_FOR_EACH(_LTZ_PP_VA_JOIN_I_FE_OP, sep_idx, BOOST_PP_SEQ_TAIL(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#endif