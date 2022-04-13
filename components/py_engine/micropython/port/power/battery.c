#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#if 0
#include "amp_defines.h"
#include "amp_system.h"
#include "amp_pm.h"
#endif

extern const mp_obj_type_t power_battery_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_battery_obj_t;

void battery_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_battery_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t battery_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_battery_obj_t* driver_obj = m_new_obj(mp_battery_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &power_battery_type;
    driver_obj->ModuleName = "battery";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_battery_obj_t* driver_obj = (mp_battery_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_battery_obj_t* driver_obj = (mp_battery_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_close, 1, obj_close);

STATIC mp_obj_t obj_getConnectState(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int state;
#if 0
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_battery_obj_t* driver_obj = (mp_battery_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (amp_battery_connect_state_get(&state))
    {
        LOG_E("get battery connect state fail\n");
    }
    LOG_D("%s: state = %d;\n", __func__, state);
#endif
    return MP_ROM_INT(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_getConnectState, 1, obj_getConnectState);

STATIC mp_obj_t obj_getVoltage(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int voltage;
#if 0
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_battery_obj_t* driver_obj = (mp_battery_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (amp_battery_voltage_get(&voltage))
    {
        LOG_E("get battery connect voltage fail\n");
    }
    LOG_D("%s: voltage = %d;\n", __func__, voltage);
#endif
    return MP_ROM_INT(voltage);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_getVoltage, 1, obj_getVoltage);

STATIC mp_obj_t obj_getLevel(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int level;
#if 0
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_battery_obj_t* driver_obj = (mp_battery_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (amp_battery_level_get(&level))
    {
        LOG_E("get battery connect level fail\n");
    }
    LOG_D("%s: level = %d;\n", __func__, level);
#endif
    return MP_ROM_INT(level);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_getLevel, 1, obj_getLevel);

STATIC mp_obj_t obj_getTemperature(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int temperature;
#if 0
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_battery_obj_t* driver_obj = (mp_battery_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (amp_battery_temperature_get(&temperature))
    {
        LOG_E("get battery connect temperature fail\n");
    }
    LOG_D("%s: temperature = %d;\n", __func__, temperature);
#endif
    return MP_ROM_INT(temperature);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_getTemperature, 1, obj_getTemperature);

STATIC const mp_rom_map_elem_t battery_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_Battery)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&battery_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&battery_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_getConnectState), MP_ROM_PTR(&battery_obj_getConnectState)},
    {MP_ROM_QSTR(MP_QSTR_getVoltage), MP_ROM_PTR(&battery_obj_getVoltage)},
    {MP_ROM_QSTR(MP_QSTR_getLevel), MP_ROM_PTR(&battery_obj_getLevel)},
    {MP_ROM_QSTR(MP_QSTR_getTemperature), MP_ROM_PTR(&battery_obj_getTemperature)},
};

STATIC MP_DEFINE_CONST_DICT(battery_locals_dict, battery_locals_dict_table);

const mp_obj_type_t power_battery_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Battery,
    .print = battery_obj_print,
    .make_new = battery_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&battery_locals_dict,
};

