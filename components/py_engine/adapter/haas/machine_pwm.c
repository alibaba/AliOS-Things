/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "aos_hal_pwm.h"
#include "modmachine.h"
#include "mphalport.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "machine_pwm"

enum {
    PWN_CHANNEL_0,
    PWN_CHANNEL_1,
    PWN_CHANNEL_2,
    PWN_CHANNEL_3,
    PWN_CHANNEL_MAX
};

// Forward dec'l
extern const mp_obj_type_t machine_pwm_type;

// Params for PW operation
#define PWFREQ (5000)
#define PWDUTY (50)

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    pwm_dev_t dev;
    mp_uint_t duty_cycle;
    mp_uint_t freq;
} machine_pwm_obj_t;

STATIC int update_param(machine_pwm_obj_t *self, mp_uint_t newfreq, mp_uint_t newduty)
{
    int status = -1;
    pwm_dev_t *dev = &self->dev;

    mp_uint_t oduty = self->duty_cycle;
    mp_uint_t ofreq = self->freq;

    pwm_config_t cfg = {
        .duty_cycle = newduty / 100.f,
        .freq = newfreq,
    };

    status = aos_hal_pwm_para_chg(dev, cfg);
    if (status != 0) {
        LOGE(LOG_TAG, "aos_hal_pwm_para_chg failed, status=%d\n", status);
        return status;
    }

    self->freq = newfreq;
    self->duty_cycle = newduty / 100.f;
    self->dev.config.freq = self->freq;
    self->dev.config.duty_cycle = self->duty_cycle;

    return status;
}

/******************************************************************************/

// MicroPython bindings for PWM

STATIC void machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%u", self->id);
    mp_printf(print, ", freq=%u, duty=%u)", self->freq, self->duty_cycle);
}

STATIC void machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum {
        ARG_freq,
        ARG_duty
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_duty, MP_ARG_INT, { .u_int = PWDUTY } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pwm_dev_t *dev = &self->dev;

    // Set freq and duty cycle?
    mp_int_t freq = args[ARG_freq].u_int;
    mp_int_t duty = args[ARG_duty].u_int;

    /* if freq inited */
    if (freq != -1) {
        dev->config.duty_cycle = duty / 100.f;
        dev->config.freq = freq;

        int status = aos_hal_pwm_init(dev);
        if (status != 0) {
            LOGE(LOG_TAG, "aos_hal_pwm_init failed, status=%d\n", status);
            return;
        }

        status = aos_hal_pwm_start(dev);
        if (status != 0) {
            LOGE(LOG_TAG, "aos_hal_pwm_start failed, status=%d\n", status);
        }
    }
}

STATIC mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get PWM id
    mp_int_t pwm_id = mp_obj_get_int(args[0]);
    if (pwm_id < 0 || pwm_id >= PWN_CHANNEL_MAX) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM(%d) does not exist"), pwm_id);
    }

    // create PWM object from the given pin
    machine_pwm_obj_t *self = m_new_obj(machine_pwm_obj_t);
    self->base.type = &machine_pwm_type;
    self->id = pwm_id;
    self->duty_cycle = PWDUTY / 100.f;
    self->freq = PWFREQ;
    self->dev.port = pwm_id;
    self->dev.config.duty_cycle = self->duty_cycle;
    self->dev.config.freq = self->freq;

    // start the PWM running for this channel
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_pwm_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    machine_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pwm_init_obj, 1, machine_pwm_init);

STATIC mp_obj_t machine_pwm_deinit(mp_obj_t self_in)
{
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    aos_hal_pwm_stop(&self->dev);
    aos_hal_pwm_finalize(&self->dev);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_deinit_obj, machine_pwm_deinit);

STATIC mp_obj_t machine_pwm_freq(size_t n_args, const mp_obj_t *args)
{
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(self->freq);
    }

    // set
    int freq = mp_obj_get_int(args[1]);
    int status = update_param(self, freq, self->duty_cycle);
    return MP_OBJ_NEW_SMALL_INT(status);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_freq_obj, 1, 2, machine_pwm_freq);

STATIC mp_obj_t machine_pwm_duty(size_t n_args, const mp_obj_t *args)
{
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->duty_cycle);
    }

    // set
    mp_int_t duty = mp_obj_get_int(args[1]);
    mp_int_t status = update_param(self, self->freq, duty);
    return MP_OBJ_NEW_SMALL_INT(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_obj, 1, 2, machine_pwm_duty);

STATIC mp_obj_t machine_pwm_freqduty(size_t n_args, const mp_obj_t *args)
{
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    mp_int_t freq = mp_obj_get_int(args[1]);
    mp_int_t duty = mp_obj_get_int(args[2]);

    int status = update_param(self, freq, duty);
    return MP_OBJ_NEW_SMALL_INT(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_freqduty_obj, 1, 2, machine_pwm_freqduty);

STATIC const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&machine_pwm_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_freqduty), MP_ROM_PTR(&machine_pwm_freqduty_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

const mp_obj_type_t machine_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = machine_pwm_print,
    .make_new = machine_pwm_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pwm_locals_dict,
};
