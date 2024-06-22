#ifndef LTZ_PROC_INIT_PTR_TO_VOID_HPP
#define LTZ_PROC_INIT_PTR_TO_VOID_HPP

#include <map>
#include <string>
#include <boost/preprocessor.hpp>

namespace ltz {
namespace proc_init {
namespace pv {

inline void **get_ppv(const std::string &name) {
    static std::map<std::string, void **> ppv_map;
    auto it = ppv_map.find(name);
    if (it == ppv_map.end()) {
        it = ppv_map.emplace(name, new void *{nullptr}).first;
    }
    return it->second;
}
}  // namespace pv
}  // namespace proc_init
}  // namespace ltz

#define _LTZ_PI_PV_GET_PPV_I(_pv) ::ltz::proc_init::pv::get_ppv(BOOST_PP_STRINGIZE(_pv))
#define _LTZ_PI_PV_HANDLER(handler, pv) BOOST_PP_CAT(pv, _handler_##handler)
#define _LTZ_PI_PV_HANDLER_OBJ(handler, pv) BOOST_PP_CAT(pv, _handler_obj_##handler)
#define _LTZ_PI_PV_HANDLE_FN(handler, pv) BOOST_PP_CAT(pv, _handle_fn_##handler)

#define _LTZ_PI_PV_HANDLE_I(name, pv)                                 \
    void _LTZ_PI_PV_HANDLE_FN(name, pv)(void **);                     \
    struct _LTZ_PI_PV_HANDLER(name, pv) {                             \
        _LTZ_PI_PV_HANDLER(name, pv)() {                              \
            _LTZ_PI_PV_HANDLE_FN(name, pv)(_LTZ_PI_PV_GET_PPV_I(pv)); \
        }                                                             \
    } _LTZ_PI_PV_HANDLER_OBJ(name, pv);                               \
    void _LTZ_PI_PV_HANDLE_FN(name, pv)(void **lpi_ppv)


/* ********** */

/* export */

/*
    @brief Define function body to handle ptr to ptr to void, which is guaranteed to be initialized.
    @param name unique name for pv that identify the handler
    @param pv unique name that identify the void ptr, which is to be handle.
    @details Prototype after macro expasnsion:
        void pv_handle_fn_name(void **lpi_ppv);
 */
#define LTZ_PI_PV_HANDLE(name, pv) _LTZ_PI_PV_HANDLE_I(name, pv)

/*
    @brief Get ptr to ptr to void by pv
 */
#define LTZ_PI_PV_GET_PPV(pv) _LTZ_PI_PV_GET_PPV_I(pv)

#endif