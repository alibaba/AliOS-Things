#include <stdio.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

extern const mp_obj_type_t pyb_ipc_type;
extern void hal_reboot(void);
STATIC mp_obj_t machine_reset(void)
{
    hal_reboot();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);


STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_machine)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ipc), MP_ROM_PTR(&pyb_ipc_type)},
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t machine_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};


