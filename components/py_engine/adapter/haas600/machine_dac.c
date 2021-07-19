/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
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

#if MICROPY_PY_MACHINE_HW_PWM

#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

#include "ulog/ulog.h"
#include "aos_hal_dac.h"

#define LOG_TAG "machine_dac"

enum {
    DAC_CHANNEL_0,
    DAC_CHANNEL_1,
    DAC_CHANNEL_MAX
};

typedef struct _mdac_obj_t {
    mp_obj_base_t base;
    dac_dev_t dev;
    uint8_t channel_id;
} mdac_obj_t;

const mp_obj_type_t machine_dac_type;

STATIC mdac_obj_t mdac_obj[] = {
    {{&machine_dac_type}, {DAC_CHANNEL_0, NULL},  DAC_CHANNEL_0},
    {{&machine_dac_type}, {DAC_CHANNEL_1, NULL},  DAC_CHANNEL_1},
};

STATIC mp_obj_t mdac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    mp_arg_check_num(n_args, n_kw, 1, 1, true);

    int32_t channel_id = mp_obj_get_int(args[0]);
    mdac_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(mdac_obj); i++) {
        if (channel_id == mdac_obj[i].channel_id) {
            self = &mdac_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("ChannelId[%d] Should Smaller Than %d"), channel_id, DAC_CHANNEL_MAX);
    }
    
    int status = aos_hal_dac_init(&self->dev);
    if(status != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to init DAC, parameter error"));
    }

    status = aos_hal_dac_start(&self->dev, self->channel_id);
    if (0 != status) {
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to start DAC, parameter error"));
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mdac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mdac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "DAC(Channel(%u))", self->channel_id);
}

STATIC mp_obj_t mdac_write(mp_obj_t self_in, mp_obj_t value_in) {
    mdac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int value = mp_obj_get_int(value_in);
    if (value < 0 || value > 255) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range"));
    }

    int status = aos_hal_dac_set_value(&self->dev, self->channel_id, value);
    if (status != 0) {
        LOGE(LOG_TAG, "DAC write fail!");
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_write_obj, mdac_write);

STATIC const mp_rom_map_elem_t mdac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mdac_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mdac_locals_dict, mdac_locals_dict_table);

const mp_obj_type_t machine_dac_type = {
    { &mp_type_type },
    .name = MP_QSTR_DAC,
    .print = mdac_print,
    .make_new = mdac_make_new,
    .locals_dict = (mp_obj_t)&mdac_locals_dict,
};

#endif