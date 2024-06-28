#ifndef LTZ_PREPROCESSOR_VARIADIC_ENCLOSE_PARAMS_HPP
#define LTZ_PREPROCESSOR_VARIADIC_ENCLOSE_PARAMS_HPP

#include <boost/preprocessor.hpp>

/*
    @brief Enclose each parameter in variadic to string.
    @param ... Variadic to enclose
    @return Variadic enclosed
 */
#define LTZ_PP_VA_ENCLOSE_PARAMS(...) _LTZ_PP_VA_ENCLOSE_PARAMS_I(__VA_ARGS__)


/* definition */
#define _LTZ_PP_VA_ENCLOSE_PARAMS_I_OP(s, data, elem) BOOST_PP_STRINGIZE(elem)
#define _LTZ_PP_VA_ENCLOSE_PARAMS_I(...) BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(_LTZ_PP_VA_ENCLOSE_PARAMS_I_OP, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#endif
