/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#if MICROPY_PY_MACHINE_DAC

#include <stdio.h>

#include "aos_hal_dac.h"
#include "modmachine.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "machine_dac"

#define MDAC_CHECK_PARAMS()                                  \
    mdac_obj_t *self = (mdac_obj_t *)MP_OBJ_TO_PTR(self_in); \
    do {                                                     \
        if (self == NULL) {                                  \
            mp_raise_OSError(MP_EINVAL);                     \
            return mp_const_none;                            \
        }                                                    \
    } while (0)

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
    { { &machine_dac_type }, { DAC_CHANNEL_0, NULL }, DAC_CHANNEL_0 },
    { { &machine_dac_type }, { DAC_CHANNEL_1, NULL }, DAC_CHANNEL_1 },
};

STATIC mp_obj_t mdac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, true);

    mp_int_t channel_id = mp_obj_get_int(args[0]);
    mdac_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(mdac_obj); i++) {
        if (channel_id == mdac_obj[i].channel_id) {
            self = &mdac_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("ChannelId[%d] Should Smaller Than %d"), channel_id,
                          DAC_CHANNEL_MAX);
    }

    int status = aos_hal_dac_init(&self->dev);
    if (status != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to init DAC, parameter error"));
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mdac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    MDAC_CHECK_PARAMS();
    mp_printf(print, "DAC(Channel(%u))", self->channel_id);
}

STATIC mp_obj_t mdac_write(mp_obj_t self_in, mp_obj_t value_in)
{
    MDAC_CHECK_PARAMS();

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

STATIC mp_obj_t mdac_read(mp_obj_t self_in)
{
    MDAC_CHECK_PARAMS();

    mp_int_t res = aos_hal_dac_get_value(&self->dev, self->channel_id);
    return MP_OBJ_NEW_SMALL_INT(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_read_obj, mdac_read);

STATIC mp_obj_t mdac_start(mp_obj_t self_in)
{
    MDAC_CHECK_PARAMS();

    mp_int_t status = aos_hal_dac_start(&self->dev, self->channel_id);
    return MP_OBJ_NEW_SMALL_INT(status);
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_start_obj, mdac_start);

STATIC mp_obj_t mdac_stop(mp_obj_t self_in)
{
    MDAC_CHECK_PARAMS();

    mp_int_t status = aos_hal_dac_stop(&self->dev, self->channel_id);
    return MP_OBJ_NEW_SMALL_INT(status);
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_stop_obj, mdac_stop);

STATIC mp_obj_t mdac_deinit(mp_obj_t self_in)
{
    MDAC_CHECK_PARAMS();

    mp_int_t status = aos_hal_dac_finalize(&self->dev);
    return MP_OBJ_NEW_SMALL_INT(status);
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_deinit_obj, mdac_deinit);

STATIC const mp_rom_map_elem_t mdac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&mdac_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&mdac_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mdac_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mdac_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mdac_deinit_obj) },
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