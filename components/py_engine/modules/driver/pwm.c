#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos_hal_pwm.h"
#include "board_mgr.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "DRIVER_PWM"

extern const mp_obj_type_t driver_pwm_type;

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t pwm_handle;
} mp_pwm_obj_t;

void pwm_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t pwm_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_pwm_obj_t *driver_obj = m_new_obj(mp_pwm_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(MP_ENOMEM);
    }

    driver_obj->Base.type = &driver_pwm_type;
    driver_obj->ModuleName = "pwm";
    driver_obj->pwm_handle.handle = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;

    if (n_args < 2) {
        LOGE(LOG_TAG, "args num is illegal :n_args = %d;\n", n_args);
        mp_raise_OSError(MP_EINVAL);
    }

    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t *driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        mp_raise_OSError(MP_EINVAL);
    }

    char *id = (char *)mp_obj_str_get_str(args[1]);
    if (id == NULL) {
        LOGE(LOG_TAG, "illegal id: %s;\n", id);
        mp_raise_OSError(MP_EINVAL);
    }

    ret = py_board_mgr_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "py_board_mgr_init failed\n");
        return MP_ROM_INT(ret);
        ;
    }

    ret = py_board_attach_item(MODULE_PWM, id, &(driver_obj->pwm_handle));
    if (ret != 0) {
        LOGE(LOG_TAG, "py_board_attach_item failed ret = %d;\n", ret);
        goto out;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
        LOGE(LOG_TAG, "py_board_get_node_by_handle failed;\n");
        goto out;
    }
    ret = aos_hal_pwm_init(pwm_device);
    return MP_ROM_INT(ret);

out:
    if (0 != ret) {
        LOGE(LOG_TAG, "%s: aos_hal_pwm_init failed ret = %d;\n", __func__, ret);
        py_board_disattach_item(MODULE_PWM, &(driver_obj->pwm_handle));
    }

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 1) {
        LOGE(LOG_TAG, "args num is illegal :n_args = %d;\n", n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t *driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-MP_EINVAL);
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
        LOGE(LOG_TAG, "py_board_get_node_by_handle failed;\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_stop(pwm_device);
    ret |= aos_hal_pwm_finalize(pwm_device);
    py_board_disattach_item(MODULE_PWM, &(driver_obj->pwm_handle));

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_close, 1, obj_close);

STATIC mp_obj_t obj_setOption(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 2) {
        LOGE(LOG_TAG, "args num is illegal :n_args = %d;\n", n_args);
        return MP_ROM_INT(-MP_EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t *driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-MP_EINVAL);
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
        LOGE(LOG_TAG, "py_board_get_node_by_handle failed;\n");
        goto out;
    }

    if (!mp_obj_is_dict_or_ordereddict(args[1])) {
        LOGE(LOG_TAG, "%s  param1 type error,param type must be dict \r\n",
                  __func__);
        goto out;
    }

    mp_obj_t index = mp_obj_new_str_via_qstr("duty", 4);
    pwm_device->config.duty_cycle =  (float)mp_obj_get_int(mp_obj_dict_get(args[1], index)) / 100.0;

    index = mp_obj_new_str_via_qstr("freq", 4);
    pwm_device->config.freq = mp_obj_get_int(mp_obj_dict_get(args[1], index));

    ret = aos_hal_pwm_stop(pwm_device);
    if (ret != 0) {
        LOGE(LOG_TAG, "aos_hal_pwm_stop failed\n");
        goto out;
    }


    pwm_config_t para;
    para.duty_cycle = pwm_device->config.duty_cycle;
    para.freq = pwm_device->config.freq;

    ret = aos_hal_pwm_para_chg(pwm_device, para);
    if (ret != 0) {
        LOGE(LOG_TAG, "amp hal pwm init failed\n");
        goto out;
    }

    ret = aos_hal_pwm_start(pwm_device);
    if (ret != 0) {
        LOGE(LOG_TAG, "aos_hal_pwm_start failed\n");
    }
out:
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_setOption, 2, obj_setOption);


STATIC mp_obj_t obj_getOption(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 1) {
        LOGE(LOG_TAG, "args num is illegal :n_args = %d;\n", n_args);
        return MP_ROM_INT(-MP_EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t *driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-MP_EINVAL);
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
        LOGE(LOG_TAG, "py_board_get_node_by_handle failed;\n");
        return MP_ROM_INT(-MP_EINVAL);
    }
    mp_obj_t dict = MP_OBJ_NULL;
    dict = mp_obj_new_dict(2);
    mp_obj_dict_store(dict, mp_obj_new_str("freq", 4),
                            mp_obj_new_int((int)(pwm_device->config.freq)));
    mp_obj_dict_store(dict, mp_obj_new_str("duty", 4),
                            mp_obj_new_int((int)(pwm_device->config.duty_cycle * 100)));

    return dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_getOption, 1, obj_getOption);

STATIC const mp_rom_map_elem_t pwm_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_PWM) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&pwm_obj_open) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&pwm_obj_close) },
    { MP_ROM_QSTR(MP_QSTR_getOption), MP_ROM_PTR(&pwm_obj_getOption) },
    { MP_ROM_QSTR(MP_QSTR_setOption), MP_ROM_PTR(&pwm_obj_setOption) },

};

STATIC MP_DEFINE_CONST_DICT(pwm_locals_dict, pwm_locals_dict_table);

const mp_obj_type_t driver_pwm_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = pwm_obj_print,
    .make_new = pwm_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&pwm_locals_dict,
};
