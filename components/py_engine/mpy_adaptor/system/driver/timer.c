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

#include "HaasLog.h"
#include "k_api.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "aos/hal/timer.h"

#define TIMER_MODE_ONE_SHOT (TIMER_RELOAD_MANU)
#define TIMER_MODE_PERIODIC (TIMER_RELOAD_AUTO)
#define TIMER_DEFAULT_PERIOD (1000000U)
typedef struct _driver_timer_obj_t {
    mp_obj_base_t base;
    mp_uint_t port;
    timer_dev_t *device;
    mp_obj_t callback;
} driver_timer_obj_t;

const mp_obj_type_t driver_timer_type;

STATIC void driver_timer_disable(driver_timer_obj_t *self);

STATIC timer_dev_t *driver_timer_get_devive(driver_timer_obj_t *self) {
    if (self == NULL) {
        LOG_E("driver_timer_obj is NULL\n");
        return NULL;
    }

    if(self->device != NULL) {
        return self->device;
    } else {
        timer_dev_t *device = aos_calloc(1, sizeof(timer_dev_t));
        if (NULL == device) {
            LOG_E("%s: timer_dev_t calloc failed;\n", __func__);
            goto fail;
        }
        device->port = self->port;
        self->device = device;
    }

    return self->device;

fail:
    self->device = NULL;
    return NULL;
}

STATIC void driver_timer_print(const mp_print_t *print, mp_obj_t self_in,
                               mp_print_kind_t kind) {
    driver_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "period=%d, ", self->device->config.period);
    mp_printf(print, "reload_mode=%d, ", self->device->config.reload_mode);
}

STATIC mp_obj_t driver_timer_make_new(const mp_obj_type_t *type, size_t n_args,
                                       size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_uint_t port = mp_obj_get_int(args[0]);

    driver_timer_obj_t *driver_obj = m_new_obj(driver_timer_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
        return mp_const_none;
    }
    driver_obj->base.type = &driver_timer_type;
    driver_obj->port = port;
    driver_obj->device = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC void driver_timer_disable(driver_timer_obj_t *self) {
    timer_dev_t *tim = driver_timer_get_devive(self);
    hal_timer_stop(tim);
}

STATIC void driver_timer_isr(void *self_in) {
    driver_timer_obj_t *self = (driver_timer_obj_t*)self_in;
    mp_obj_t callback = self->callback;
    if (self->callback != mp_const_none) {
        callback_to_python(self->callback, self);
    }
}

STATIC void driver_timer_enable(driver_timer_obj_t *self) {
    timer_dev_t *tim = driver_timer_get_devive(self);
    if(tim != NULL) {
        hal_timer_init(tim);
        hal_timer_start(tim);
    }
}

STATIC mp_obj_t driver_timer_init_helper(driver_timer_obj_t *self,
                                          mp_uint_t n_args,
                                          const mp_obj_t *pos_args,
                                          mp_map_t *kw_args) {
    enum
    {
        ARG_index,
        ARG_mode,
        ARG_callback,
        ARG_period,
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_index, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
        {MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC}},
        {MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none}},
        {MP_QSTR_period, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_DEFAULT_PERIOD}},
    };

    driver_timer_disable(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    if (self == NULL || self->device == NULL) {
        LOG_E("%s: hal_timer_init failed \n", __func__);
        return mp_const_none;
    }

    self->callback = args[ARG_callback].u_obj;
    self->device->port = (int8_t)args[ARG_index].u_int;
    self->device->config.reload_mode = args[ARG_mode].u_int;
    self->device->config.period = (uint32_t)args[ARG_period].u_int;
    self->device->config.arg = self;
    self->device->config.cb = driver_timer_isr;

    driver_timer_enable(self);
    return mp_const_none;
}

STATIC mp_obj_t driver_timer_deinit(mp_obj_t self_in) {
    driver_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    driver_timer_disable(self);
    timer_dev_t *tim = driver_timer_get_devive(self);
    if(tim != NULL) {
        aos_free(tim);
        self->device = NULL;
    }
    hal_timer_stop(tim);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(driver_timer_deinit_obj, driver_timer_deinit);

STATIC mp_obj_t driver_timer_init(mp_uint_t n_args, const mp_obj_t *args,
                                   mp_map_t *kw_args) {
    return driver_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(driver_timer_init_obj, 1, driver_timer_init);

STATIC mp_obj_t driver_timer_period(mp_obj_t self_in, mp_const_obj_t period)
{
    driver_timer_obj_t *self = self_in;
    if(self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_const_none;
    } else {
        self->device->config.period = mp_obj_get_int(period);
    }
    timer_config_t para = self->device->config;

    timer_dev_t *tim = driver_timer_get_devive(self);
    int32_t ret = hal_timer_para_chg(tim, para);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(driver_timer_period_obj, driver_timer_period);

STATIC mp_obj_t driver_timer_start(mp_obj_t self_in) {
    driver_timer_obj_t *self = self_in;
    timer_dev_t *tim = driver_timer_get_devive(self);
    int32_t ret = hal_timer_start(tim);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(driver_timer_start_obj, driver_timer_start);

STATIC mp_obj_t driver_timer_stop(mp_obj_t self_in) {
    driver_timer_obj_t *self = self_in;
    timer_dev_t *tim = driver_timer_get_devive(self);
    hal_timer_stop(tim);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(driver_timer_stop_obj, driver_timer_stop);

STATIC const mp_rom_map_elem_t driver_timer_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&driver_timer_deinit_obj)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&driver_timer_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&driver_timer_deinit_obj)},
    {MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&driver_timer_start_obj)},
    {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&driver_timer_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_period), MP_ROM_PTR(&driver_timer_period_obj)},

    {MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONE_SHOT)},
    {MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC)},
};
STATIC MP_DEFINE_CONST_DICT(driver_timer_locals_dict, driver_timer_locals_dict_table);

const mp_obj_type_t driver_timer_type = {
    {&mp_type_type},
    .name = MP_QSTR_TIMER,
    .print = driver_timer_print,
    .make_new = driver_timer_make_new,
    .locals_dict = (mp_obj_t)&driver_timer_locals_dict,
};
