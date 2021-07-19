#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include "ulog/ulog.h"

#define LOG_TAG "MOD_BATTERY"

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_close, 1, obj_close);

STATIC mp_obj_t obj_getConnectState(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int state;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    //if (amp_battery_connect_state_get(&state))
    {
        LOGE(LOG_TAG, "get battery connect state fail\n");
    }
    LOGD(LOG_TAG, "%s: state = %d;\n", __func__, state);

    return MP_ROM_INT(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_getConnectState, 1, obj_getConnectState);

STATIC mp_obj_t obj_getVoltage(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int voltage;

    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    //if (amp_battery_voltage_get(&voltage))
    {
        LOGE(LOG_TAG, "get battery connect voltage fail\n");
    }
    LOGD(LOG_TAG, "%s: voltage = %d;\n", __func__, voltage);

    return MP_ROM_INT(voltage);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_getVoltage, 1, obj_getVoltage);

STATIC mp_obj_t obj_getLevel(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int level;

    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    //if (amp_battery_level_get(&level))
    {
        LOGE(LOG_TAG, "get battery connect level fail\n");
    }
    LOGD(LOG_TAG, "%s: level = %d;\n", __func__, level);

    return MP_ROM_INT(level);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(battery_obj_getLevel, 1, obj_getLevel);

STATIC mp_obj_t obj_getTemperature(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int temperature;

    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    //if (amp_battery_temperature_get(&temperature))
    {
        LOGE(LOG_TAG, "get battery connect temperature fail\n");
    }
    LOGD(LOG_TAG, "%s: temperature = %d;\n", __func__, temperature);

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

const mp_obj_module_t battery_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&battery_locals_dict,
};
