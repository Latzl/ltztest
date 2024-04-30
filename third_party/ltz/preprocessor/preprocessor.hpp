#ifndef LTZ_PREPROCESSOR_HPP
#define LTZ_PREPROCESSOR_HPP

#include <boost/preprocessor.hpp>

#define LTZ_PP_CAT_WITH_SEP_OP(s, sep, elem) BOOST_PP_CAT(sep, elem)

#define LTZ_PP_CAT_WITH_SEP(sep, ...) BOOST_PP_SEQ_CAT(BOOST_PP_SEQ_PUSH_FRONT(BOOST_PP_SEQ_POP_FRONT(BOOST_PP_SEQ_TRANSFORM(LTZ_PP_CAT_WITH_SEP_OP, sep, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))), BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)))

#define LTZ_PP_CAT_WITH_UNDERLINE(...) LTZ_PP_CAT_WITH_SEP(_, __VA_ARGS__)
#define LTZ_PP_CAT_WITH_SLASH(...) LTZ_PP_CAT_WITH_SEP(/, __VA_ARGS__)
#define LTZ_PP_CAT_WITH_DOT(...) LTZ_PP_CAT_WITH_SEP(., __VA_ARGS__)
#define LTZ_PP_CAT_WITH_DASH(...) LTZ_PP_CAT_WITH_SEP(-, __VA_ARGS__)
#define LTZ_PP_CAT_WITH_PIPE(...) LTZ_PP_CAT_WITH_SEP(|, __VA_ARGS__)
#define LTZ_PP_CAT_WITH_COLON(...) LTZ_PP_CAT_WITH_SEP( :, __VA_ARGS__)
#define LTZ_PP_CAT_WITH_AND(...) LTZ_PP_CAT_WITH_SEP(&, __VA_ARGS__)
#define LTZ_PP_CAT_WITH_AT(...) LTZ_PP_CAT_WITH_SEP(@, __VA_ARGS__)

// void LTZ_PP_CAT_WITH_UNDERLINE(ltz, pp, cat, with, underline)();

// #define macro2(sep, ...) BOOST_PP_SEQ_TRANSFORM(LTZ_PP_CAT_WITH_SEP_OP, sep, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
// // macro2(_,a,b,c)
// #define macro1(sep, ...) BOOST_PP_SEQ_PUSH_FRONT(BOOST_PP_SEQ_POP_FRONT(macro2(sep, __VA_ARGS__)), BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__))
// // macro1(_,a,b,c)
// #define macro0(sep, ...) BOOST_PP_SEQ_CAT(macro1(sep, __VA_ARGS__))
// // macro0(_,a,b,c)

// const std::string ltzppstr = BOOST_PP_STRINGIZE(macro2(_,a,b,c));
// // void BOOST_PP_SEQ_CAT(macro2(_, ltz, pp))();

#endif