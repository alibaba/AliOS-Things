/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "k_api.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ulog/ulog.h"

#define LOG_TAG              "machine_sw_timer"

#define TIMER_MODE_ONESHOT   (0)
#define TIMER_MODE_PERIODIC  (1)
#define TIMER_DEFAULT_PERIOD (1000U)

typedef struct _machine_soft_timer_obj_t {
    mp_obj_base_t base;
    mp_uint_t port;
    mp_uint_t period;
    mp_uint_t reload_mode;
    void *arg;
    aos_timer_t *timerId;
    mp_obj_t callback;
} machine_soft_timer_obj_t;

const mp_obj_type_t mp_machine_soft_timer_type;

STATIC void machine_soft_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    machine_soft_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "port=%d, ", self->port);
    mp_printf(print, "period=%d, ", self->period);
    mp_printf(print, "reload_mode=%d, ", self->reload_mode);
}

STATIC mp_obj_t machine_soft_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_uint_t port = mp_obj_get_int(args[0]);

    machine_soft_timer_obj_t *swtimer_obj = m_new_obj(machine_soft_timer_obj_t);
    if (!swtimer_obj) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }
    memset(swtimer_obj, 0, sizeof(machine_soft_timer_obj_t));

    swtimer_obj->base.type = &mp_machine_soft_timer_type;
    swtimer_obj->port = port;

    return MP_OBJ_FROM_PTR(swtimer_obj);
}

STATIC void machine_soft_timer_enable(machine_soft_timer_obj_t *self)
{
    aos_timer_t *tim = self->timerId;
    aos_timer_start(tim);
}

STATIC void machine_soft_timer_disable(machine_soft_timer_obj_t *self)
{
    aos_timer_t *tim = self->timerId;
    aos_timer_stop(tim);
}

STATIC void machine_soft_timer_isr(void *timer, void *self_in)
{
    machine_soft_timer_obj_t *self = (machine_soft_timer_obj_t *)MP_OBJ_TO_PTR(self_in);
    mp_obj_t callback = self->callback;
    if (callback != mp_const_none && mp_obj_is_callable(callback)) {
        callback_to_python(callback, self);
    }
}

STATIC mp_obj_t machine_soft_timer_init_helper(machine_soft_timer_obj_t *self, mp_uint_t n_args,
                                               const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum {
        ARG_mode,
        ARG_callback,
        ARG_period,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = AOS_TIMER_REPEAT | AOS_TIMER_AUTORUN } },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_period, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = TIMER_DEFAULT_PERIOD } },
    };

    machine_soft_timer_disable(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (self == NULL) {
        LOGD(LOG_TAG, "%s: machine_soft_timer_init_helper failed\n", __func__);
        return mp_const_none;
    }

    self->callback = args[ARG_callback].u_obj;

    self->reload_mode = args[ARG_mode].u_int;
    self->period = (mp_uint_t)args[ARG_period].u_int;
    self->arg = (void *)self;

    mp_int_t status = aos_timer_create(self->timerId, machine_soft_timer_isr, self->arg, self->period,
                                       (self->reload_mode | AOS_TIMER_AUTORUN));
    if (status != 0) {
        LOGE(LOG_TAG, "%s, %d, aos_timer_create failed\n", __func__, __LINE__);
        return mp_const_none;
    }

    return mp_const_none;
}

STATIC mp_obj_t machine_soft_timer_deinit(mp_obj_t self_in)
{
    machine_soft_timer_obj_t *self = (machine_soft_timer_obj_t *)MP_OBJ_TO_PTR(self_in);
    machine_soft_timer_disable(self);
    aos_timer_free(self->timerId);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_soft_timer_deinit_obj, machine_soft_timer_deinit);

STATIC mp_obj_t machine_soft_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return machine_soft_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_soft_timer_init_obj, 1, machine_soft_timer_init);

STATIC mp_obj_t machine_soft_timer_period(mp_obj_t self_in, mp_obj_t period)
{
    machine_soft_timer_obj_t *self = (machine_soft_timer_obj_t *)MP_OBJ_TO_PTR(self_in);
    if (self_in == NULL) {
        mp_raise_OSError(ENOENT);
        return mp_const_none;
    } else {
        self->period = mp_obj_get_int(period);
    }

    mp_int_t ret = aos_timer_change(self->timerId, self->period);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_soft_timer_period_obj, machine_soft_timer_period);

STATIC mp_obj_t machine_soft_timer_start(mp_obj_t self_in)
{
    machine_soft_timer_obj_t *self = (machine_soft_timer_obj_t *)MP_OBJ_TO_PTR(self_in);
    mp_int_t ret = aos_timer_start(self->timerId);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_soft_timer_start_obj, machine_soft_timer_start);

STATIC mp_obj_t machine_soft_timer_stop(mp_obj_t self_in)
{
    machine_soft_timer_obj_t *self = (machine_soft_timer_obj_t *)MP_OBJ_TO_PTR(self_in);
    aos_timer_stop(self->timerId);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_soft_timer_stop_obj, machine_soft_timer_stop);

STATIC mp_obj_t machine_soft_timer_value(mp_obj_t self_in)
{
    machine_soft_timer_obj_t *self = (machine_soft_timer_obj_t *)MP_OBJ_TO_PTR(self_in);

    uint64_t time_ns[4] = { 0 };
    aos_timer_gettime(self->timerId, time_ns);

    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)(time_ns[0] * 1000 + time_ns[1] / 10000000));  // value in ms
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_soft_timer_value_obj, machine_soft_timer_value);

STATIC const mp_rom_map_elem_t machine_soft_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_soft_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_soft_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_soft_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_soft_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_soft_timer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_period), MP_ROM_PTR(&machine_soft_timer_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_soft_timer_value_obj) },

    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
STATIC MP_DEFINE_CONST_DICT(machine_soft_timer_locals_dict, machine_soft_timer_locals_dict_table);

const mp_obj_type_t mp_machine_soft_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_SoftTimer,
    .print = machine_soft_timer_print,
    .make_new = machine_soft_timer_make_new,
    .locals_dict = (mp_obj_t)&machine_soft_timer_locals_dict,
};
