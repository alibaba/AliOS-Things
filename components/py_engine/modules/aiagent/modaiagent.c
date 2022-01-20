#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_AIAGENT

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

extern const mp_obj_type_t aiagent_type;

// this is the actual C-structure for our new object
STATIC const mp_rom_map_elem_t aiagent_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_aiagent) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AIAgent), MP_ROM_PTR(&aiagent_type) },
};

STATIC MP_DEFINE_CONST_DICT(aiagent_locals_dict, aiagent_locals_dict_table);

const mp_obj_module_t aiagent_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&aiagent_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_aiagent, aiagent_module, MICROPY_PY_AIAGENT);

#endif
