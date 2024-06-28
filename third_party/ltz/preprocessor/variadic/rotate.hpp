#ifndef LTZ_PREPROCESSOR_VARIADIC_ROTATE_HPP
#define LTZ_PREPROCESSOR_VARIADIC_ROTATE_HPP

#include <boost/preprocessor.hpp>

/*
    @brief Rotate right n bits
    @param n Number of bits to rotate, n must >= 0.
    @param ... Variadic to rotate
    @return Variadict rotated
 */
#define LTZ_PP_VA_ROR(n, ...) _LTZ_PP_VA_ROR_I(n, __VA_ARGS__)

/*
    @brief Rotate left n bits
 */
#define LTZ_PP_VA_ROL(n, ...) _LTZ_PP_VA_ROL_I(n, __VA_ARGS__)


/* definition */
#define _LTZ_PP_VA_ROR_I_SHIFT_CNT1(n, size) BOOST_PP_MOD(n, size)
#define _LTZ_PP_VA_ROR_I_SHIFT_CNT(n, list) _LTZ_PP_VA_ROR_I_SHIFT_CNT1(n, BOOST_PP_LIST_SIZE(list))
#define _LTZ_PP_VA_ROR_I_FIRST(n, list) BOOST_PP_LIST_FIRST_N(BOOST_PP_SUB(BOOST_PP_LIST_SIZE(list), _LTZ_PP_VA_ROR_I_SHIFT_CNT(n, list)), list)
#define _LTZ_PP_VA_ROR_I_REST(n, list) BOOST_PP_LIST_REST_N(BOOST_PP_SUB(BOOST_PP_LIST_SIZE(list), _LTZ_PP_VA_ROR_I_SHIFT_CNT(n, list)), list)
#define _LTZ_PP_VA_ROR_I_SHIFTED(n, list) BOOST_PP_LIST_APPEND(_LTZ_PP_VA_ROR_I_REST(n, list), _LTZ_PP_VA_ROR_I_FIRST(n, list))
#define _LTZ_PP_VA_ROR_I(n, ...) BOOST_PP_LIST_ENUM(_LTZ_PP_VA_ROR_I_SHIFTED(n, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)))

#define _LTZ_PP_VA_ROL_I(n, ...) _LTZ_PP_VA_ROR_I(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), _LTZ_PP_VA_ROR_I_SHIFT_CNT1(n, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))), __VA_ARGS__)


#endif