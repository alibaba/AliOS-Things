/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <string.h>

#include "aos_hal_wdg.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG             "machine-wdt"

#define DEFAULT_WDT_TIMEOUT (5000)

const mp_obj_type_t machine_wdt_type;

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    wdg_dev_t dev;
} machine_wdt_obj_t;

STATIC machine_wdt_obj_t wdt_default = { { &machine_wdt_type }, { 0, { DEFAULT_WDT_TIMEOUT }, NULL } };

STATIC mp_obj_t machine_wdt_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args)
{
    enum {
        ARG_id,
        ARG_timeout
    };
    static const mp_arg_t allowed_args[] = { { MP_QSTR_id, MP_ARG_INT, { .u_int = 0 } },
                                             { MP_QSTR_timeout, MP_ARG_INT, { .u_int = DEFAULT_WDT_TIMEOUT } } };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_id].u_int != 0) {
        mp_raise_ValueError(NULL);
    }

    // Convert milliseconds to seconds
    args[ARG_timeout].u_int /= 1000;

    if (args[ARG_timeout].u_int <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    }

    wdg_dev_t *dev = &(wdt_default.dev);
    wdg_config_t cfg = { 0 };
    cfg.timeout = args[ARG_timeout].u_int;
    dev->config = cfg;

    mp_int_t rs_code = aos_hal_wdg_init(dev);
    if (rs_code != 0) {
        mp_raise_OSError(rs_code);
    }

    return &wdt_default;
}

STATIC mp_obj_t machine_wdt_feed(mp_obj_t self_in)
{
    machine_wdt_obj_t *self = (machine_wdt_obj_t *)self_in;
    aos_hal_wdg_reload(&self->dev);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_wdt_feed_obj, machine_wdt_feed);

STATIC mp_obj_t machine_wdt_deinit(mp_obj_t self_in)
{
    machine_wdt_obj_t *self = (machine_wdt_obj_t *)self_in;
    mp_int_t status = aos_hal_wdg_finalize(&self->dev);
    return MP_OBJ_NEW_SMALL_INT(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_wdt_deinit_obj, machine_wdt_deinit);

STATIC const mp_rom_map_elem_t machine_wdt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&machine_wdt_feed_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_wdt_deinit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_wdt_locals_dict, machine_wdt_locals_dict_table);

const mp_obj_type_t machine_wdt_type = {
    { &mp_type_type },
    .name = MP_QSTR_WDT,
    .make_new = machine_wdt_make_new,
    .locals_dict = (mp_obj_t)&machine_wdt_locals_dict,
};
