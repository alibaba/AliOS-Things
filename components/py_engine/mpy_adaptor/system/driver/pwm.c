#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "board_mgr.h"
#include "aos_hal_pwm.h"

extern const mp_obj_type_t driver_pwm_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t pwm_handle;
} mp_pwm_obj_t;

void pwm_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t pwm_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_pwm_obj_t* driver_obj = m_new_obj(mp_pwm_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_pwm_type;
    driver_obj->ModuleName = "pwm";
    driver_obj->pwm_handle.handle     = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t* driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    char *id = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("%s: id =%s;\n", __func__, id);

    if (id == NULL)
    {
        LOG_E("%s:illegal par id =%s;\n", __func__, id);
        return mp_const_none;
    }

    ret = py_board_mgr_init();
    if (ret != 0)
    {
        LOG_E("%s:py_board_mgr_init failed\n", __func__);
        return mp_const_none;
    }

    LOG_D("%s: py_board_mgr_init ret = %d;\n", __func__, ret);
    ret = py_board_attach_item(MODULE_PWM, id, &(driver_obj->pwm_handle));
    if (ret != 0)
    {
        LOG_E("%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    LOG_D("%s: port = %d;\n", __func__, pwm_device->port);
    LOG_D("%s: duty_cycle = %f;\n", __func__, pwm_device->config.duty_cycle);
    LOG_D("%s: freq = %d;\n", __func__, pwm_device->config.freq);
    ret = aos_hal_pwm_init(pwm_device);

out:
	if (0 != ret) {
        LOG_E("%s: aos_hal_pwm_init failed ret = %d;\n", __func__, ret);
		py_board_disattach_item(MODULE_PWM, &(driver_obj->pwm_handle));
	}

    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t* driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    ret = aos_hal_pwm_stop(pwm_device);
    ret |= aos_hal_pwm_finalize(pwm_device);
    py_board_disattach_item(MODULE_PWM, &(driver_obj->pwm_handle));
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_close, 1, obj_close);

STATIC mp_obj_t obj_getFreq(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t* driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }
    LOG_D("%s: duty_cycle = %f;\n", __func__, pwm_device->config.duty_cycle);
    LOG_D("%s: freq = %d;\n", __func__, pwm_device->config.freq);

    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(pwm_device->config.freq);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_getFreq, 1, obj_getFreq);

STATIC mp_obj_t obj_getDuty(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t* driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }
    LOG_D("%s: duty_cycle = %f;\n", __func__, pwm_device->config.duty_cycle);
    LOG_D("%s: freq = %d;\n", __func__, pwm_device->config.freq);

    LOG_D("%s:out\n", __func__);

    return mp_obj_new_float(pwm_device->config.duty_cycle);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_getDuty, 1, obj_getDuty);

STATIC mp_obj_t obj_setFreq(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t* driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    pwm_device->config.freq = (uint32_t)mp_obj_get_int(args[1]);
    LOG_D("%s: duty_cycle = %f;\n", __func__, pwm_device->config.duty_cycle);
    LOG_D("%s: freq = %d;\n", __func__, pwm_device->config.freq);

    ret = aos_hal_pwm_stop(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_stop failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_finalize(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_finalize failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_init(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_stop failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_start(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_start failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_setFreq, 2, obj_setFreq);

STATIC mp_obj_t obj_setDuty(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t* driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    pwm_device->config.duty_cycle = (float)mp_obj_get_float(args[1]);
    LOG_D("%s: duty_cycle = %f;\n", __func__, pwm_device->config.duty_cycle);
    LOG_D("%s: freq = %d;\n", __func__, pwm_device->config.freq);

    ret = aos_hal_pwm_stop(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_stop failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_finalize(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_finalize failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_init(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_stop failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_start(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_start failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_setDuty, 2, obj_setDuty);

STATIC mp_obj_t obj_setConfig(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    pwm_dev_t *pwm_device = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_pwm_obj_t* driver_obj = (mp_pwm_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    pwm_device = py_board_get_node_by_handle(MODULE_PWM, &(driver_obj->pwm_handle));
    if (NULL == pwm_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    pwm_device->config.freq = (uint32_t)mp_obj_get_int(args[1]);
    pwm_device->config.duty_cycle = (float)mp_obj_get_float(args[2]);
    LOG_D("%s: duty_cycle = %f;\n", __func__, pwm_device->config.duty_cycle);
    LOG_D("%s: freq = %d;\n", __func__, pwm_device->config.freq);

    ret = aos_hal_pwm_stop(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_stop failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_finalize(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_finalize failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_init(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_stop failed\n");
        return mp_const_none;
    }

    ret = aos_hal_pwm_start(pwm_device);
    if (ret != 0)
    {
        LOG_E("aos_hal_pwm_start failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(pwm_obj_setConfig, 3, obj_setConfig);
STATIC const mp_rom_map_elem_t pwm_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_PWM)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&pwm_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&pwm_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_getDuty), MP_ROM_PTR(&pwm_obj_getDuty)},
    {MP_ROM_QSTR(MP_QSTR_getFreq), MP_ROM_PTR(&pwm_obj_getFreq)},
    {MP_ROM_QSTR(MP_QSTR_setConfig), MP_ROM_PTR(&pwm_obj_setConfig)},
    {MP_ROM_QSTR(MP_QSTR_setFreq), MP_ROM_PTR(&pwm_obj_setFreq)},
    {MP_ROM_QSTR(MP_QSTR_setDuty), MP_ROM_PTR(&pwm_obj_setDuty)},
};

STATIC MP_DEFINE_CONST_DICT(pwm_locals_dict, pwm_locals_dict_table);

const mp_obj_type_t driver_pwm_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_PWM,
    .print = pwm_obj_print,
    .make_new = pwm_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&pwm_locals_dict,
};

