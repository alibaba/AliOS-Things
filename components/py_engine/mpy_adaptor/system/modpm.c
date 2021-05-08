#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_open, 5, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_close, 5, obj_close);

STATIC mp_obj_t obj_setAutosleepMode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_setAutosleepMode, 5, obj_setAutosleepMode);

STATIC mp_obj_t obj_getAutosleepMode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_getAutosleepMode, 5, obj_getAutosleepMode);

STATIC mp_obj_t obj_sleep(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_sleep, 5, obj_sleep);

STATIC mp_obj_t obj_powerDown(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_powerDown, 5, obj_powerDown);

STATIC mp_obj_t obj_powerReset(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_powerReset, 5, obj_powerReset);

STATIC mp_obj_t obj_wakelockLock(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_wakelockLock, 5, obj_wakelockLock);

STATIC mp_obj_t obj_wakelockUnlock(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_wakelockUnlock, 5, obj_wakelockUnlock);

STATIC mp_obj_t obj_wakelockTimedlock(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_wakelockTimedlock, 5, obj_wakelockTimedlock);

STATIC mp_obj_t obj_onPwrkey(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pm_obj_onPwrkey, 5, obj_onPwrkey);

STATIC const mp_rom_map_elem_t pm_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_PM)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&pm_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&pm_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_setAutosleepMode), MP_ROM_PTR(&pm_obj_setAutosleepMode)},
    {MP_ROM_QSTR(MP_QSTR_getAutosleepMode), MP_ROM_PTR(&pm_obj_getAutosleepMode)},
    {MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&pm_obj_sleep)},
    {MP_ROM_QSTR(MP_QSTR_powerDown), MP_ROM_PTR(&pm_obj_powerDown)},
    {MP_ROM_QSTR(MP_QSTR_powerReset), MP_ROM_PTR(&pm_obj_powerReset)},
    {MP_ROM_QSTR(MP_QSTR_wakelockLock), MP_ROM_PTR(&pm_obj_wakelockLock)},
    {MP_ROM_QSTR(MP_QSTR_wakelockUnlock), MP_ROM_PTR(&pm_obj_wakelockUnlock)},
    {MP_ROM_QSTR(MP_QSTR_wakelockTimedlock), MP_ROM_PTR(&pm_obj_wakelockTimedlock)},
    {MP_ROM_QSTR(MP_QSTR_onPwrkey), MP_ROM_PTR(&pm_obj_onPwrkey)},
};

STATIC MP_DEFINE_CONST_DICT(pm_locals_dict, pm_locals_dict_table);

const mp_obj_module_t pm_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&pm_locals_dict,
};
