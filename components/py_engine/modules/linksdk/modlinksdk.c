#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include "ulog/ulog.h"

extern const mp_obj_type_t linksdk_device_type;


STATIC const mp_rom_map_elem_t linksdk_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_linksdk)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Device), MP_ROM_PTR(&linksdk_device_type)},
};

STATIC MP_DEFINE_CONST_DICT(linksdk_locals_dict, linksdk_locals_dict_table);

const mp_obj_module_t linksdk_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&linksdk_locals_dict,
};

