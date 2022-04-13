#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t system_fs_type;
extern const mp_obj_type_t system_kv_type;
extern const mp_obj_type_t system_info_type;
extern const mp_obj_type_t system_sdcard_type;
// this is the actual C-structure for our new object

STATIC const mp_rom_map_elem_t system_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_system)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_FS), MP_ROM_PTR(&system_fs_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_KV), MP_ROM_PTR(&system_kv_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_SystemInfo), MP_ROM_PTR(&system_info_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Sdcard), MP_ROM_PTR(&system_sdcard_type)},
};

STATIC MP_DEFINE_CONST_DICT(system_locals_dict, system_locals_dict_table);

const mp_obj_module_t system_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&system_locals_dict,
};

