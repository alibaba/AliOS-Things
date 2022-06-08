#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_SYSTEM

#include "amp_utils.h"
#include "aos/kernel.h"
#include "aos_system.h"
#include "genhdr/mpversion.h"
#include "py/builtin.h"
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "MOD_SYSTEM"

STATIC const qstr os_uname_info_fields[] = { MP_QSTR_sysname, MP_QSTR_nodename, MP_QSTR_release, MP_QSTR_version,
                                             MP_QSTR_machine };
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, MICROPY_PY_SYS_NODE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_release_obj, MICROPY_VERSION_STRING);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

STATIC MP_DEFINE_ATTRTUPLE(os_uname_info_obj, os_uname_info_fields, 5, (mp_obj_t)&os_uname_info_sysname_obj,
                           (mp_obj_t)&os_uname_info_nodename_obj, (mp_obj_t)&os_uname_info_release_obj,
                           (mp_obj_t)&os_uname_info_version_obj, (mp_obj_t)&os_uname_info_machine_obj);

STATIC mp_obj_t obj_getInfo()
{
    return (mp_obj_t)&os_uname_info_obj;
}
MP_DEFINE_CONST_FUN_OBJ_0(native_get_system_info, obj_getInfo);

STATIC mp_obj_t obj_sleep(void)
{
    // aos_system_sleep();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(native_sleep_system, obj_sleep);

STATIC mp_obj_t obj_reboot(void)
{
    aos_reboot();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(native_reset_system, obj_reboot);

STATIC mp_obj_t obj_getUptime()
{
    uint64_t begin_ms = aos_now_ms();
    return MP_ROM_INT(begin_ms);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(native_get_system_uptime, obj_getUptime);

STATIC mp_obj_t obj_getMemory()
{
    int32_t ret = -1;
    amp_heap_info_t heap_info;

    ret = amp_heap_memory_info(&heap_info);
    if (ret != 0) {
        LOGE(LOG_TAG, "get heap memory failed");
        return mp_const_none;
    }
    mp_obj_t dict = mp_obj_new_dict(3);
    mp_obj_dict_store(dict, mp_obj_new_str("total", strlen("total")), mp_obj_new_int(heap_info.heap_total));
    mp_obj_dict_store(dict, mp_obj_new_str("used", strlen("used")), mp_obj_new_int(heap_info.heap_used));
    mp_obj_dict_store(dict, mp_obj_new_str("free", strlen("free")), mp_obj_new_int(heap_info.heap_free));
    return dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(native_get_memory_info, obj_getMemory);

STATIC const mp_rom_map_elem_t system_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_system) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&native_get_system_info) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&native_sleep_system) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reboot), MP_ROM_PTR(&native_reset_system) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getUptime), MP_ROM_PTR(&native_get_system_uptime) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getMemory), MP_ROM_PTR(&native_get_memory_info) },
};

STATIC MP_DEFINE_CONST_DICT(system_module_globals, system_module_globals_table);

const mp_obj_module_t system_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&system_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_system, system_module, MICROPY_PY_SYSTEM);

#endif  // MICROPY_PY_SYSTEM