#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t power_pm_type;
extern const mp_obj_type_t power_charger_type;
extern const mp_obj_type_t power_battery_type;
// this is the actual C-structure for our new object

STATIC const mp_rom_map_elem_t power_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_power)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Battery), MP_ROM_PTR(&power_battery_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Charger), MP_ROM_PTR(&power_charger_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_PM), MP_ROM_PTR(&power_pm_type)},
};

STATIC MP_DEFINE_CONST_DICT(power_locals_dict, power_locals_dict_table);

const mp_obj_module_t power_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&power_locals_dict,
};

