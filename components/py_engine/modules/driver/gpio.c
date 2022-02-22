#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos_hal_gpio.h"
#include "board_mgr.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ulog/ulog.h"

#define LOG_TAG "DRIVER_GPIO"

extern const mp_obj_type_t driver_gpio_type;

typedef enum {
    IRQ_RISING_EDGE  = 0x1, /**< Interrupt triggered at input signal's rising edge  */
    IRQ_FALLING_EDGE = 0x2, /**< Interrupt triggered at input signal's falling edge */
    IRQ_BOTH_EDGES   = IRQ_RISING_EDGE | IRQ_FALLING_EDGE,
} gpio_irq_trigger_params_t;

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device;
    mp_obj_t callback;
} mp_gpio_obj_t;

void gpio_driver_irq_handler(int polarity, void *arg)
{
    mp_gpio_obj_t *self = (mp_gpio_obj_t *)arg;
    int32_t level = aos_hal_gpio_get(self->gpio_device);
    callback_to_python_LoBo(self->callback, MP_OBJ_NEW_SMALL_INT(level), NULL);
}

void gpio_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t gpio_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_gpio_obj_t *driver_obj = m_new_obj(mp_gpio_obj_t);
    if (!driver_obj) {
        LOGE(LOG_TAG, "%s: alloc mem failed;\n", __func__);
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_gpio_type;
    driver_obj->ModuleName = "gpio";
    driver_obj->gpio_handle.handle = NULL;
    driver_obj->gpio_device = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 2) {
        LOGE(LOG_TAG, "%s: illegal args;\n", __func__);
        mp_raise_OSError(EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t *driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        mp_raise_OSError(EINVAL);
    }

    if (driver_obj->gpio_device != NULL) {
        LOGE(LOG_TAG, "device hass opened\n");
        return MP_ROM_INT(-ENXIO);
    }

    int ret = -1;
    gpio_dev_t *gpio_device = NULL;
    char *id = (char *)mp_obj_str_get_str(args[1]);

    if (id == NULL) {
        LOGE(LOG_TAG, "%s:illegal par id =%s;\n", __func__, id);
        mp_raise_OSError(EINVAL);
    }

    if (py_board_mgr_init() != 0) {
        LOGE(LOG_TAG, "%s:py_board_mgr_init failed\n", __func__);
        mp_raise_OSError(EINVAL);
    }

    if (py_board_attach_item(MODULE_GPIO, id, &(driver_obj->gpio_handle)) != 0) {
        LOGE(LOG_TAG, "%s: py_board_attach_item failed ret = %d;\n", __func__, ret);
        py_board_disattach_item(MODULE_GPIO, &(driver_obj->gpio_handle));
        mp_raise_OSError(EINVAL);
    }

    gpio_device = py_board_get_node_by_handle(MODULE_GPIO, &(driver_obj->gpio_handle));
    if (NULL == gpio_device) {
        LOGE(LOG_TAG, "%s: py_board_get_node_by_handle failed;\n", __func__);
        py_board_disattach_item(MODULE_GPIO, &(driver_obj->gpio_handle));
        gpio_device = NULL;
        mp_raise_OSError(EINVAL);
    }
    driver_obj->gpio_device = gpio_device;


    ret = aos_hal_gpio_init(gpio_device);
    if (ret) {
        mp_raise_OSError(ret);
    }

    return MP_ROM_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t *driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-EINVAL);
    }
    if (NULL == driver_obj->gpio_device) {
        LOGE(LOG_TAG, "driver_obj has closed\n");
        return MP_ROM_INT(-ENXIO);
    }

    ret = aos_hal_gpio_finalize(driver_obj->gpio_device);
    if (ret != 0) {
        LOGE(LOG_TAG, "%s: aos_hal_gpio_finalize failed;\n", __func__);
        return MP_ROM_INT(-EINVAL);
    }
    py_board_disattach_item(MODULE_GPIO, &(driver_obj->gpio_handle));
    driver_obj->gpio_device = NULL;
    return MP_ROM_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_close, 1, obj_close);

STATIC mp_obj_t obj_read(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    uint32_t level = 0;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t *driver_obj = (mp_gpio_obj_t *)self;
    if (NULL == driver_obj) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-EINVAL);
    }
    if (NULL == driver_obj->gpio_device) {
        LOGE(LOG_TAG, "driver_obj has closed\n");
        return MP_ROM_INT(-ENXIO);
    }

    level = aos_hal_gpio_get(driver_obj->gpio_device);

    return MP_ROM_INT(level);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_read, 1, obj_read);

STATIC mp_obj_t obj_write(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    uint32_t level = 0;
    if (n_args < 2) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t *driver_obj = (mp_gpio_obj_t *)self;
    if (NULL == driver_obj) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-EINVAL);
    }
    if (NULL == driver_obj->gpio_device) {
        LOGE(LOG_TAG, "driver_obj has closed\n");
        return MP_ROM_INT(-ENXIO);
    }

    level = (uint32_t)mp_obj_get_int(args[1]);

    if (level) {
        ret = aos_hal_gpio_output_high(driver_obj->gpio_device);
    } else {
        ret = aos_hal_gpio_output_low(driver_obj->gpio_device);
    }

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_write, 2, obj_write);

STATIC mp_obj_t obj_toggle(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    if (n_args < 1) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t *driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-EINVAL);
    }

    ret = aos_hal_gpio_output_toggle(driver_obj->gpio_device);
    if (ret == -1) {
        LOGE(LOG_TAG, "aos_hal_gpio_output_toggle failed\n");
    }

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(gpio_obj_toggle, 1, obj_toggle);

STATIC mp_obj_t obj_on(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    uint32_t irq_edge = 0;
    if (n_args < 2 || args[1] == mp_const_none) {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return MP_ROM_INT(-EINVAL);
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_gpio_obj_t *driver_obj = (mp_gpio_obj_t *)self;
    if (driver_obj == NULL) {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return MP_ROM_INT(-EINVAL);
    }
    if (driver_obj->gpio_device == NULL) {
        LOGE(LOG_TAG, "driver_obj has closed\n");
        return MP_ROM_INT(-ENXIO);
    }

    driver_obj->callback = args[1];

    // judge callback function if callback == None disable IQR
    // ret = aos_hal_gpio_clear_irq(driver_obj->gpio_device);
    gpio_params_t *priv = (gpio_params_t *)driver_obj->gpio_device->priv;

    driver_obj->callback = args[1];
    if (n_args == 3) {
        irq_edge = (uint32_t)mp_obj_get_int(args[2]);
    } else {
        irq_edge = priv->irq_mode;
    }

    ret = aos_hal_gpio_enable_irq(driver_obj->gpio_device, (int8_t)irq_edge, gpio_driver_irq_handler, (void *)driver_obj);
    if (ret < 0) {
        LOGE(LOG_TAG, "%s:aos_hal_gpio_enable_irq failed, %d\n", __func__, ret);
    }

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gpio_on_obj, 2, 3, obj_on);


STATIC const mp_rom_map_elem_t gpio_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_GPIO) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&gpio_obj_open) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&gpio_obj_close) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&gpio_obj_read) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&gpio_obj_write) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&gpio_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_RisingEdge), MP_ROM_INT(IRQ_RISING_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_FallingEdge), MP_ROM_INT(IRQ_FALLING_EDGE) },
};

STATIC MP_DEFINE_CONST_DICT(gpio_locals_dict, gpio_locals_dict_table);

const mp_obj_type_t driver_gpio_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_GPIO,
    .print = gpio_obj_print,
    .make_new = gpio_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&gpio_locals_dict,
};
