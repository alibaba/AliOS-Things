/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos_hal_timer.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ulog/ulog.h"
#include "utility.h"

#define LOG_TAG              "machine_timer"

#define TIMER_MODE_ONESHOT   (TIMER_RELOAD_MANU)
#define TIMER_MODE_PERIODIC  (TIMER_RELOAD_AUTO)
#define TIMER_DEFAULT_PERIOD (1000000U)
typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    mp_uint_t port;
    timer_dev_t dev;
    mp_obj_t callback;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

STATIC void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "port=%d, ", self->port);
    mp_printf(print, "period=%d, ", self->dev.config.period);
    mp_printf(print, "reload_mode=%d, ", self->dev.config.reload_mode);
}

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_uint_t port = mp_obj_get_int(args[0]);

    machine_timer_obj_t *timer_obj = m_new_obj(machine_timer_obj_t);
    if (!timer_obj) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }
    memset(timer_obj, 0, sizeof(machine_timer_obj_t));

    timer_obj->base.type = &machine_timer_type;
    timer_obj->port = port;
    timer_obj->dev.port = port;

    return MP_OBJ_FROM_PTR(timer_obj);
}

STATIC void machine_timer_disable(timer_dev_t *tim)
{
    aos_hal_timer_stop(tim);
}

STATIC mp_int_t aos_timer_reload(timer_dev_t *tim)
{
    return aos_hal_timer_reload(tim);
}

STATIC void machine_timer_isr(void *self_in)
{
    machine_timer_obj_t *self = (machine_timer_obj_t *)self_in;
    mp_obj_t callback = self->callback;
    if (callback != mp_const_none && mp_obj_is_callable(callback)) {
        callback_to_python(callback, self);
    }
}

STATIC void machine_timer_enable(machine_timer_obj_t *self)
{
    timer_dev_t *tim = &self->dev;

    if (tim != NULL) {
        aos_hal_timer_init(tim);
        aos_hal_timer_start(tim);
    }
}

STATIC mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args,
                                          mp_map_t *kw_args)
{
    enum {
        ARG_period,
        ARG_mode,
        ARG_callback,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_period, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = TIMER_DEFAULT_PERIOD } },
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = TIMER_MODE_PERIODIC } },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };

    machine_timer_disable(&self->dev);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (self == NULL) {
        LOGE(LOG_TAG, "%s: hal_timer_init failed\n", __func__);
        return mp_const_none;
    }

    self->callback = args[ARG_callback].u_obj;

    self->dev.port = self->port;
    self->dev.config.reload_mode = args[ARG_mode].u_int;
    self->dev.config.period = (mp_uint_t)args[ARG_period].u_int * 1000UL;
    self->dev.config.arg = self;
    self->dev.config.cb = machine_timer_isr;

    machine_timer_enable(self);
    return mp_const_none;
}

STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in)
{
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    timer_dev_t *tim = &self->dev;
    aos_hal_timer_stop(tim);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return machine_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

STATIC mp_obj_t machine_timer_period(mp_obj_t self_in, mp_obj_t period)
{
    machine_timer_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    } else {
        self->dev.config.period = mp_obj_get_int(period);
    }
    timer_config_t para = self->dev.config;

    timer_dev_t *tim = &self->dev;
    mp_int_t ret = aos_hal_timer_para_chg(tim, para);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_timer_period_obj, machine_timer_period);

STATIC mp_obj_t machine_timer_start(mp_obj_t self_in)
{
    machine_timer_obj_t *self = self_in;
    timer_dev_t *tim = &self->dev;
    mp_int_t ret = aos_hal_timer_start(tim);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_start_obj, machine_timer_start);

STATIC mp_obj_t machine_timer_stop(mp_obj_t self_in)
{
    machine_timer_obj_t *self = self_in;
    timer_dev_t *tim = &self->dev;
    aos_hal_timer_stop(tim);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_stop_obj, machine_timer_stop);

STATIC mp_obj_t machine_timer_reload(mp_obj_t self_in)
{
    machine_timer_obj_t *self = self_in;
    timer_dev_t *tim = &self->dev;
    mp_int_t ret = aos_timer_reload(tim);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_reload_obj, machine_timer_reload);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_timer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_period), MP_ROM_PTR(&machine_timer_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_reload), MP_ROM_PTR(&machine_timer_reload_obj) },

    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = machine_timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_t)&machine_timer_locals_dict,
};
