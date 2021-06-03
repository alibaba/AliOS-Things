#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "k_api.h"
#include "HaasLog.h"
#include "board_mgr.h"
#include "aos_hal_gpio.h"

extern const mp_obj_type_t driver_gpio_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t gpio_handle;
    mp_obj_t callback;
} mp_gpio_obj_t;

void gpio_driver_irq_handler(void *arg)
{
    mp_gpio_obj_t *self = (mp_gpio_obj_t *)arg;
    if (self->callback != mp_const_none) {
        callback_to_python(self->callback, self);
    }
}

void gpio_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t gpio_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_gpio_obj_t* driver_obj = m_new_obj(mp_gpio_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_gpio_type;
    driver_obj->ModuleName = "gpio";
    driver_obj->gpio_handle.handle     = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    gpio_dev_t *gpio_device = NULL;

    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
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
    ret = py_board_attach_item(MODULE_GPIO, id, &(driver_obj->gpio_handle));
    if (ret != 0)
    {
        LOG_E("%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        goto out;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        goto out;
    }

    LOG_D("%s: port = %d;\n", __func__, gpio_device->port);
    LOG_D("%s: config = %d;\n", __func__, gpio_device->config);
    ret = aos_hal_gpio_init(gpio_device);

out:
	if (0 != ret) {
        LOG_E("%s: aos_hal_gpio_init failed ret = %d;\n", __func__, ret);
		py_board_disattach_item(MODULE_GPIO, &(driver_obj->gpio_handle));
	}

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    gpio_dev_t *gpio_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    ret = aos_hal_gpio_finalize(gpio_device);
    if(ret != 0)
    {
		LOG_E("%s: aos_hal_gpio_finalize failed;\n", __func__);
        return mp_const_none;
    }
    py_board_disattach_item(MODULE_GPIO, &(driver_obj->gpio_handle));
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_close, 1, obj_close);

STATIC mp_obj_t obj_read(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    uint32_t level = 0;
    gpio_dev_t *gpio_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    //hal_gpio_input_get(gpio_device, &level);
    aos_hal_gpio_input_get(gpio_device, &level);
    //aos_hal_gpio_get(gpio_device, &level);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(level);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_read, 1, obj_read);

STATIC mp_obj_t obj_write(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    uint32_t level  = 0;
    gpio_dev_t *gpio_device = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    level = (uint32_t)mp_obj_get_int(args[1]);
    LOG_D("%s:level = %d;\n", __func__, level);

    if (level)
    {
        ret = aos_hal_gpio_output_high(gpio_device);
        LOG_E("GPIO %d OFF \n", gpio_device->port);
    }
    else
    {
        ret = aos_hal_gpio_output_low(gpio_device);
        LOG_E("GPIO %d ON \n", gpio_device->port);
    }
    if (ret == -1)
    {
        LOG_E("aos_hal_gpio_output failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_write, 2, obj_write);

STATIC mp_obj_t obj_toggle(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    gpio_dev_t *gpio_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    ret = aos_hal_gpio_output_toggle(gpio_device);
    if (ret == -1)
    {
        LOG_E("aos_hal_gpio_output_toggle failed\n");
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_toggle, 5, obj_toggle);

STATIC mp_obj_t obj_enableIrq(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    int8_t irq_edge = 0;
    gpio_dev_t *gpio_device = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    gpio_params_t *priv = (gpio_params_t *)gpio_device->priv;
    if (priv == NULL)
    {
        LOG_E("priv is NULL\n");
        return mp_const_none;
    }

    driver_obj->callback = args[1];
    irq_edge = priv->irq_mode;
    LOG_D("%s:irq_edge = %d;\n", __func__, irq_edge);
    ret = aos_hal_gpio_enable_irq(gpio_device, (int8_t)irq_edge, gpio_driver_irq_handler, (void*)driver_obj);
    if (ret < 0)
    {
        LOG_E("%s:aos_hal_gpio_enable_irq failed\n", __func__);
    }
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_enableIrq, 2, obj_enableIrq);

STATIC mp_obj_t obj_disableIrq(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    gpio_dev_t *gpio_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    ret = aos_hal_gpio_disable_irq(gpio_device);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_disableIrq, 1, obj_disableIrq);

STATIC mp_obj_t obj_clearIrq(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    gpio_dev_t *gpio_device = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t* driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
		LOG_E("%s: py_board_get_node_by_handle failed;\n", __func__);
        return mp_const_none;
    }

    ret = aos_hal_gpio_clear_irq(gpio_device);
    LOG_D("%s:out\n", __func__);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_clearIrq, 1, obj_clearIrq);

STATIC const mp_rom_map_elem_t gpio_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_GPIO)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&gpio_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&gpio_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&gpio_obj_read)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&gpio_obj_write)},
    {MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&gpio_obj_toggle)},
    {MP_ROM_QSTR(MP_QSTR_enableIrq), MP_ROM_PTR(&gpio_obj_enableIrq)},
    {MP_ROM_QSTR(MP_QSTR_disableIrq), MP_ROM_PTR(&gpio_obj_disableIrq)},
    {MP_ROM_QSTR(MP_QSTR_clearIrq), MP_ROM_PTR(&gpio_obj_clearIrq)},
};

STATIC MP_DEFINE_CONST_DICT(gpio_locals_dict, gpio_locals_dict_table);

const mp_obj_type_t driver_gpio_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_GPIO,
    .print = gpio_obj_print,
    .make_new = gpio_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&gpio_locals_dict,
};

