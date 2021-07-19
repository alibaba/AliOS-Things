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


#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

#include "ulog/ulog.h"
#include "aos_hal_adc.h"

#define LOG_TAG "machine_adc"

#define SAMPLEING_CYCLE  1200

enum {
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_MAX
};

enum {
    ADC_WIDTH_9Bit,
    ADC_WIDTH_10Bit,
    ADC_WIDTH_11Bit,
    ADC_WIDTH_12Bit,
    ADC_WIDTH_13Bit,
};

typedef struct _madc_obj_t {
    mp_obj_base_t base;
    adc_dev_t dev;
    uint8_t adc_bit_width;
    uint8_t channel_id;
} madc_obj_t;

const mp_obj_type_t machine_adc_type;

STATIC madc_obj_t madc_obj[] = {
    {{&machine_adc_type}, {ADC_CHANNEL_0, { SAMPLEING_CYCLE }, NULL}, ADC_WIDTH_12Bit, ADC_CHANNEL_0},
    {{&machine_adc_type}, {ADC_CHANNEL_1, { SAMPLEING_CYCLE }, NULL}, ADC_WIDTH_12Bit, ADC_CHANNEL_1},
};

STATIC mp_obj_t madc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);

    int32_t channel_id = mp_obj_get_int(args[0]);
    madc_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(madc_obj); i++) {
        if (channel_id == madc_obj[i].channel_id) {
            self = &madc_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("ChannelId[%d] Should Smaller Than %d"), channel_id, ADC_CHANNEL_MAX);
    }
    
    int status = aos_hal_adc_init(&self->dev);
    if(status != 0) {
      mp_raise_ValueError(MP_ERROR_TEXT("Failed to init ADC, parameter error"));
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC void madc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADC(Channel(%u))", self->channel_id);
}

// read_u16()
STATIC mp_obj_t madc_read_u16(mp_obj_t self_in) {
    madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t output = 0;
    int status = aos_hal_adc_value_get(&self->dev, &output, 0);
    if (status != 0) {
        LOGE(LOG_TAG, "ADC read fail with status=%d\n", status);
    }
    return MP_OBJ_NEW_SMALL_INT(output);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_u16_obj, madc_read_u16);

// Legacy method
STATIC mp_obj_t madc_read(mp_obj_t self_in) {
    return madc_read_u16(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(madc_read_obj, madc_read);

STATIC mp_obj_t madc_atten(mp_obj_t self_in, mp_obj_t atten_in) {
    madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t atten = mp_obj_get_int(atten_in);

    mp_raise_NotImplementedError(MP_ERROR_TEXT("attenuation not supported"));
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_atten_obj, madc_atten);

STATIC mp_obj_t madc_width(mp_obj_t self_in, mp_obj_t width_in) {
    madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t width = mp_obj_get_int(width_in);
    switch (width) {
        case ADC_WIDTH_9Bit:
            self->adc_bit_width = 9;
            break;
        case ADC_WIDTH_10Bit:
            self->adc_bit_width = 10;
            break;
        case ADC_WIDTH_11Bit:
            self->adc_bit_width = 11;
            break;
        case ADC_WIDTH_12Bit:
            self->adc_bit_width = 12;
            break;
        case ADC_WIDTH_13Bit:
            self->adc_bit_width = 13;
            break;
        default:
            break;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_width_obj, madc_width);

STATIC const mp_rom_map_elem_t madc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&madc_read_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&madc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_atten), MP_ROM_PTR(&madc_atten_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&madc_width_obj) },

    { MP_ROM_QSTR(MP_QSTR_WIDTH_9BIT), MP_ROM_INT(ADC_WIDTH_9Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_10BIT), MP_ROM_INT(ADC_WIDTH_10Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_11BIT), MP_ROM_INT(ADC_WIDTH_11Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_12BIT), MP_ROM_INT(ADC_WIDTH_12Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_13BIT), MP_ROM_INT(ADC_WIDTH_13Bit) },
};

STATIC MP_DEFINE_CONST_DICT(madc_locals_dict, madc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = madc_print,
    .make_new = madc_make_new,
    .locals_dict = (mp_obj_t)&madc_locals_dict,
};
