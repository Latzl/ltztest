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


/* export */

#define LTZ_PP_JOIN(sep, ...) _LTZ_PP_JOIN_I(sep, __VA_ARGS__)

#define LTZ_PP_CAT_WITH_SEP(sep, ...) _LTZ_PP_CAT_WITH_SEP_I(sep, __VA_ARGS__)

#define LTZ_PP_CAT_WITH_UNDERLINE(...) LTZ_PP_CAT_WITH_SEP(_, __VA_ARGS__)

#define LTZ_PP_ENCLOSE_ELEMS(...) _LTZ_PP_ENCLOSE_ELEMS_I(__VA_ARGS__)

#endif