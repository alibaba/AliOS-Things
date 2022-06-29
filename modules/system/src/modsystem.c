/*
    HaaS Python
*/

#if MICROPY_PY_SYSTEM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "system.h"

STATIC const MP_DEFINE_STR_OBJ(version_system_obj, MICROPY_SW_VENDOR_NAME "-v" SYSINFO_SYSTEM_VERSION);

#ifdef MICROPY_HW_BOARD_TYPE
STATIC const MP_DEFINE_STR_OBJ(version_info_system_obj,
                               MICROPY_SW_VENDOR_NAME "-" MICROPY_HW_MCU_NAME "-" MICROPY_HW_BOARD_TYPE \
                               "-v" SYSINFO_SYSTEM_VERSION "-" MICROPY_BUILD_DATE);
#else
STATIC const MP_DEFINE_STR_OBJ(version_info_system_obj, MICROPY_SW_VENDOR_NAME "-" MICROPY_HW_MCU_NAME \
                               "-v" SYSINFO_SYSTEM_VERSION "-" MICROPY_BUILD_DATE);
#endif

STATIC mp_obj_t system_version(void)
{
    return MP_OBJ_FROM_PTR(&version_system_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(system_version_obj, system_version);

STATIC mp_obj_t system_version_info(void)
{
    return MP_OBJ_FROM_PTR(&version_info_system_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(system_version_info_obj, system_version_info);

STATIC const mp_rom_map_elem_t system_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_system) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&system_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_version_info), MP_ROM_PTR(&system_version_info_obj) },
};

STATIC MP_DEFINE_CONST_DICT(system_module_globals, system_module_globals_table);

const mp_obj_module_t system_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&system_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_system, system_module, MICROPY_PY_SYSTEM);

#endif  // MICROPY_PY_SYSTEM