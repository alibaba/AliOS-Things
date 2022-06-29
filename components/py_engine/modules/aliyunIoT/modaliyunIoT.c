#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#if MICROPY_PY_ALIYUNIOT

extern const mp_obj_type_t linksdk_device_type;
extern const mp_obj_type_t linksdk_gateway_type;

STATIC const mp_rom_map_elem_t linksdk_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_aliyunIoT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Device), MP_ROM_PTR(&linksdk_device_type) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Gateway), MP_ROM_PTR(&linksdk_gateway_type) },
};

STATIC MP_DEFINE_CONST_DICT(linksdk_locals_dict, linksdk_locals_dict_table);

const mp_obj_module_t mp_module_aliyunIoT = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&linksdk_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_aliyunIoT, mp_module_aliyunIoT, MICROPY_PY_ALIYUNIOT);

#endif
