#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t http_client_type;


// this is the actual C-structure for our new object

STATIC const mp_rom_map_elem_t http_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_http)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_client), MP_ROM_PTR(&http_client_type)},

};

STATIC MP_DEFINE_CONST_DICT(http_locals_dict, http_locals_dict_table);

const mp_obj_module_t http_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&http_locals_dict,
};

