/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <vfsdev/timer_dev.h>

#include "aos_hal_timer.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ulog/ulog.h"
#include "utility.h"

#define LOG_TAG              "driver_timer"

#define TIMER_MODE_ONESHOT   (TIMER_RELOAD_MANU)
#define TIMER_MODE_PERIODIC  (TIMER_RELOAD_AUTO)
#define TIMER_DEFAULT_PERIOD (1000000U)
typedef struct _driver_timer_obj_t {
    mp_obj_base_t base;
    mp_uint_t port;
    timer_dev_t dev;
    mp_obj_t callback;
} driver_timer_obj_t;

const mp_obj_type_t driver_timer_type;

STATIC void driver_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    driver_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "port=%d, ", self->port);
    mp_printf(print, "period=%d, ", self->dev.config.period);
    mp_printf(print, "reload_mode=%d, ", self->dev.config.reload_mode);
}

STATIC mp_obj_t driver_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_uint_t port = mp_obj_get_int(args[0]);

    driver_timer_obj_t *timer_obj = m_new_obj(driver_timer_obj_t);
    if (!timer_obj) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }
    memset(timer_obj, 0, sizeof(driver_timer_obj_t));

    timer_obj->base.type = &driver_timer_type;
    timer_obj->port = port;
    timer_obj->dev.port = port;

    return MP_OBJ_FROM_PTR(timer_obj);
}

STATIC void driver_timer_disable(timer_dev_t *tim)
{
    aos_hal_timer_stop(tim);
}

// STATIC mp_int_t aos_hal_timer_reload(timer_dev_t *tim)
// {
//     int32_t *p_fd = (int32_t *)tim->priv;
//     return ioctl(*p_fd, IOC_TIMER_RELOAD, (unsigned long)false);
// }

STATIC void driver_timer_isr(void *self_in)
{
    driver_timer_obj_t *self = (driver_timer_obj_t *)self_in;
    mp_obj_t callback = self->callback;
    if (callback != mp_const_none && mp_obj_is_callable(callback)) {
        callback_to_python(callback, self);
    }
}

STATIC mp_int_t driver_timer_enable(driver_timer_obj_t *self)
{
    mp_int_t ret = -1;
    timer_dev_t *tim = &self->dev;
    if (tim != NULL) {
        ret = aos_hal_timer_init(tim);
        ret = aos_hal_timer_start(tim);
    }

    return ret;
}

STATIC mp_obj_t driver_timer_init_helper(driver_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args,
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

    driver_timer_disable(&self->dev);

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
    self->dev.config.cb = driver_timer_isr;

    mp_int_t ret = driver_timer_enable(self);
    return MP_OBJ_NEW_SMALL_INT(ret);
}

STATIC mp_obj_t driver_timer_close(mp_obj_t self_in)
{
    driver_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    timer_dev_t *tim = &self->dev;
    aos_hal_timer_stop(tim);
    mp_int_t ret = aos_hal_timer_finalize(tim);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(driver_timer_close_obj, driver_timer_close);

STATIC mp_obj_t driver_timer_open(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return driver_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(driver_timer_open_obj, 1, driver_timer_open);

STATIC mp_obj_t driver_timer_period(mp_obj_t self_in, mp_obj_t period)
{
    driver_timer_obj_t *self = self_in;
    if (self_in == NULL) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    } else {
        self->dev.config.period = mp_obj_get_int(period) * 1000UL;
    }
    timer_config_t para = self->dev.config;

    timer_dev_t *tim = &self->dev;
    mp_int_t ret = aos_hal_timer_para_chg(tim, para);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(driver_timer_period_obj, driver_timer_period);

STATIC mp_obj_t driver_timer_start(mp_obj_t self_in)
{
    driver_timer_obj_t *self = self_in;
    timer_dev_t *tim = &self->dev;
    mp_int_t ret = aos_hal_timer_start(tim);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(driver_timer_start_obj, driver_timer_start);

STATIC mp_obj_t driver_timer_stop(mp_obj_t self_in)
{
    driver_timer_obj_t *self = self_in;
    timer_dev_t *tim = &self->dev;
    aos_hal_timer_stop(tim);
    return MP_OBJ_NEW_SMALL_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(driver_timer_stop_obj, driver_timer_stop);

STATIC mp_obj_t driver_timer_reload(mp_obj_t self_in)
{
    driver_timer_obj_t *self = self_in;
    timer_dev_t *tim = &self->dev;
    mp_int_t ret = aos_hal_timer_reload(tim);
    return MP_OBJ_NEW_SMALL_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(driver_timer_reload_obj, driver_timer_reload);

STATIC const mp_rom_map_elem_t driver_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&driver_timer_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&driver_timer_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&driver_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&driver_timer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_period), MP_ROM_PTR(&driver_timer_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_reload), MP_ROM_PTR(&driver_timer_reload_obj) },

    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
STATIC MP_DEFINE_CONST_DICT(driver_timer_locals_dict, driver_timer_locals_dict_table);

const mp_obj_type_t driver_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_TIMER,
    .print = driver_timer_print,
    .make_new = driver_timer_make_new,
    .locals_dict = (mp_obj_t)&driver_timer_locals_dict,
};
