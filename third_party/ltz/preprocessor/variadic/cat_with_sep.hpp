#ifndef LTZ_PREPROCESSOR_VARIADIC_CAT_WITH_SEP_HPP
#define LTZ_PREPROCESSOR_VARIADIC_CAT_WITH_SEP_HPP

#include <boost/preprocessor.hpp>

/*
    @brief Concatenate each parameter with sep.
    @param sep Separator
    @param ... Variadic to concatenate
    @return Variadic concatenated
 */
#define LTZ_PP_VA_CAT_WITH_SEP(sep, ...) _LTZ_PP_VA_CAT_WITH_SEP_I(sep, __VA_ARGS__)

#define LTZ_PP_VA_CAT_WITH_UNDERLINE(...) LTZ_PP_VA_CAT_WITH_SEP(_, __VA_ARGS__)


/* definition */
#define _LTZ_PP_VA_CAT_WITH_SEP_I_OP(s, sep, elem) BOOST_PP_CAT(sep, elem)
#define _LTZ_PP_VA_CAT_WITH_SEP_I(sep, ...) BOOST_PP_SEQ_CAT(BOOST_PP_SEQ_PUSH_FRONT(BOOST_PP_SEQ_POP_FRONT(BOOST_PP_SEQ_TRANSFORM(_LTZ_PP_VA_CAT_WITH_SEP_I_OP, sep, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))), BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)))

#endif