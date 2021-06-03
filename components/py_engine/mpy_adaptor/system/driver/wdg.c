#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "aos_hal_wdg.h"

extern const mp_obj_type_t driver_wdg_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    wdg_dev_t wdg_dev;
} mp_wdg_obj_t;

void wdg_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_wdg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t wdg_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_wdg_obj_t* driver_obj = m_new_obj(mp_wdg_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_wdg_type;
    driver_obj->ModuleName = "wdg";

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
    mp_wdg_obj_t* driver_obj = (mp_wdg_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdg_obj_open, 1, obj_open);

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
    mp_wdg_obj_t* driver_obj = (mp_wdg_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdg_obj_close, 1, obj_close);

STATIC mp_obj_t obj_start(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int32_t timeout   = 0;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wdg_obj_t* driver_obj = (mp_wdg_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    timeout = (int32_t)mp_obj_get_int(args[1]);
    wdg_dev_t *handle = (wdg_dev_t *)&(driver_obj->wdg_dev);

    LOG_D("%s:timeout = %d;\n", __func__, timeout);
    handle->config.timeout = timeout;
    ret                    = aos_hal_wdg_init(handle);
    handle->config.timeout = (ret == 0) ? timeout : 0;
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdg_obj_start, 2, obj_start);

STATIC mp_obj_t obj_stop(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wdg_obj_t* driver_obj = (mp_wdg_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    wdg_dev_t *handle = (wdg_dev_t *)&(driver_obj->wdg_dev);
    aos_hal_wdg_reload(handle);

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdg_obj_stop, 1, obj_stop);

STATIC mp_obj_t obj_feed(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wdg_obj_t* driver_obj = (mp_wdg_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    wdg_dev_t *handle = (wdg_dev_t *)&(driver_obj->wdg_dev);
    aos_hal_wdg_finalize(handle);
    handle->config.timeout = 0;
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wdg_obj_feed, 1, obj_feed);

STATIC const mp_rom_map_elem_t wdg_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_WDG)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&wdg_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&wdg_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&wdg_obj_start)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&wdg_obj_stop)},
    {MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&wdg_obj_feed)},
};

STATIC MP_DEFINE_CONST_DICT(wdg_locals_dict, wdg_locals_dict_table);

const mp_obj_type_t driver_wdg_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_WDG,
    .print = wdg_obj_print,
    .make_new = wdg_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&wdg_locals_dict,
};

