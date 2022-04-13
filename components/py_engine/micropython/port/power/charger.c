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

extern const mp_obj_type_t power_charger_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_charger_obj_t;

void charger_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_charger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t charger_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_charger_obj_t* driver_obj = m_new_obj(mp_charger_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &power_charger_type;
    driver_obj->ModuleName = "charger";

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
    mp_charger_obj_t* driver_obj = (mp_charger_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(charger_obj_open, 1, obj_open);

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
    mp_charger_obj_t* driver_obj = (mp_charger_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(charger_obj_close, 1, obj_close);

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
    mp_charger_obj_t* driver_obj = (mp_charger_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (amp_charger_connect_state_get(&state))
    {
        LOG_E("get charger connection state fail\n");
    }
    LOG_D("%s: state = %d;\n", __func__, state);
#endif
    return MP_ROM_INT(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(charger_obj_getConnectState, 1, obj_getConnectState);

STATIC mp_obj_t obj_getState(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
#if 0
    amp_charger_state_t state = AMP_CHARGER_STAT_SHUTDOWN;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_charger_obj_t* driver_obj = (mp_charger_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    //if (amp_charger_state_get(&state))
    {
        LOG_E("get charger connection state fail\n");
    }
    LOG_D("%s: state = %d;\n", __func__, state);
    ret = state;
#endif
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(charger_obj_getState, 1, obj_getState);

STATIC mp_obj_t obj_getCurrent(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int current;
#if 0
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_charger_obj_t* driver_obj = (mp_charger_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (amp_charger_current_get(&current))
    {
        LOG_E("amp_charger_current_get fail\n");
    }
    LOG_D("%s: current = %d;\n", __func__, current);
#endif
    return MP_ROM_INT(current);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(charger_obj_getCurrent, 1, obj_getCurrent);

STATIC mp_obj_t obj_switch(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = 0;
#if 0
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_charger_obj_t* driver_obj = (mp_charger_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    int val = (int)mp_obj_get_int(args[1]);
    if (amp_charger_switch_set(val))
    {
        LOG_E("amp_charger_switch_set fail\n");
        ret = -1;
    }
    LOG_D("%s: ret = %d;\n", __func__, ret);
#endif
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(charger_obj_switch, 2, obj_switch);

STATIC mp_obj_t obj_onConnect(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_charger_obj_t* driver_obj = (mp_charger_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(charger_obj_onConnect, 1, obj_onConnect);

STATIC const mp_rom_map_elem_t charger_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_Charger)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&charger_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&charger_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_getConnectState), MP_ROM_PTR(&charger_obj_getConnectState)},
    {MP_ROM_QSTR(MP_QSTR_getState), MP_ROM_PTR(&charger_obj_getState)},
    {MP_ROM_QSTR(MP_QSTR_getCurrent), MP_ROM_PTR(&charger_obj_getCurrent)},
    {MP_ROM_QSTR(MP_QSTR_switch), MP_ROM_PTR(&charger_obj_switch)},
    {MP_ROM_QSTR(MP_QSTR_onConnect), MP_ROM_PTR(&charger_obj_onConnect)},
};

STATIC MP_DEFINE_CONST_DICT(charger_locals_dict, charger_locals_dict_table);

const mp_obj_type_t power_charger_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Charger,
    .print = charger_obj_print,
    .make_new = charger_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&charger_locals_dict,
};

