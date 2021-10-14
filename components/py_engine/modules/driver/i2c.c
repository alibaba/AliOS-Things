#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos_hal_i2c.h"
#include "board_mgr.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "DRIVER_I2C"

#define I2C_CHECK_PARAMS(count)                                                       \
    if (n_args < count) {                                                             \
        LOGE(LOG_TAG, "%s: args count is illegal: n_args = %d;\n", __func__, n_args); \
        return MP_OBJ_NEW_SMALL_INT(-MP_E2BIG);                                       \
    }                                                                                 \
    mp_i2c_obj_t *driver_obj = (mp_i2c_obj_t *)MP_OBJ_TO_PTR(args[0]);                \
    do {                                                                              \
        if (driver_obj == NULL) {                                                     \
            LOGE(LOG_TAG, "driver_obj[I2C] is NULL\n");                               \
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);                                  \
        }                                                                             \
    } while (0)

#define I2C_NODE_GET()                                                                          \
    i2c_dev_t *i2c_device = py_board_get_node_by_handle(MODULE_I2C, &(driver_obj->i2c_handle)); \
    do {                                                                                        \
        if (NULL == i2c_device) {                                                               \
            LOGE(LOG_TAG, "%s: py_board_get_node_by_handle failed;\n", __func__);               \
            py_board_disattach_item(MODULE_I2C, &(driver_obj->i2c_handle));                     \
            return MP_OBJ_NEW_SMALL_INT(-MP_ENXIO);                                             \
        }                                                                                       \
    } while (0)

#define I2C_TIMEOUT_MS  (0xFFFFFF)
#define I2C_CHANNEL_MAX (4)

static bool g_is_dev_inited[I2C_CHANNEL_MAX] = { 0 };

extern const mp_obj_type_t driver_i2c_type;

// this is the actual C-structure for our new object
typedef struct {
    // base represents some basic information, like type
    mp_obj_base_t Base;
    const char *ModuleName;
    item_handle_t i2c_handle;
} mp_i2c_obj_t;

void i2c_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t i2c_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_i2c_obj_t *driver_obj = m_new_obj(mp_i2c_obj_t);
    if (!driver_obj) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Memory malloc failed"));
    }
    memset(driver_obj, 0, sizeof(mp_i2c_obj_t));

    driver_obj->Base.type = &driver_i2c_type;
    driver_obj->ModuleName = "i2c";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t i2c_open(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;

    I2C_CHECK_PARAMS(2);

    char *id = NULL;
    if (mp_obj_is_str(args[1])) {
        id = (char *)mp_obj_str_get_str(args[1]);
    } else {
        LOGE(LOG_TAG, "%s: 2nd arg should be string\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    ret = py_board_mgr_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "%s:py_board_mgr_init failed\n", __func__);
        return MP_OBJ_NEW_SMALL_INT(-MP_ENOENT);
    }

    ret = py_board_attach_item(MODULE_I2C, id, &(driver_obj->i2c_handle));
    if (ret != 0) {
        LOGE(LOG_TAG, "%s: py_board_attach_item[%s] failed, ret = %d;\n", __func__, id, ret);
        goto fail;
    }

    I2C_NODE_GET();

    LOGD(LOG_TAG, "%s: port = %d;\n", __func__, i2c_device->port);
    LOGD(LOG_TAG, "%s: address_width = %d;\n", __func__, i2c_device->config.address_width);
    LOGD(LOG_TAG, "%s: freq = %d;\n", __func__, i2c_device->config.freq);
    LOGD(LOG_TAG, "%s: mode = %d;\n", __func__, i2c_device->config.mode);
    LOGD(LOG_TAG, "%s: dev_addr = %d;\n", __func__, i2c_device->config.dev_addr);

    if (i2c_device->port >= I2C_CHANNEL_MAX) {
        ret = -MP_ENODEV;
        goto fail;
    }

    if (g_is_dev_inited[i2c_device->port] == false) {
        ret = aos_hal_i2c_init(i2c_device);
    } else {
        ret = 0;
    }

fail:
    if (ret < 0) {
        py_board_disattach_item(MODULE_I2C, &(driver_obj->i2c_handle));
    } else {
        g_is_dev_inited[i2c_device->port] = true;
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_open_obj, 2, i2c_open);

STATIC mp_obj_t i2c_close(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;

    I2C_CHECK_PARAMS(1);
    I2C_NODE_GET();

    if (g_is_dev_inited[i2c_device->port] == true) {
        ret = aos_hal_i2c_finalize(i2c_device);
        if (ret == 0) {
            g_is_dev_inited[i2c_device->port] = false;
            py_board_disattach_item(MODULE_I2C, &(driver_obj->i2c_handle));
        }
    } else {
        ret = 0;
    }

    if (ret < 0) {
        LOGE(LOG_TAG, "%s: i2c_close failed\n", __func__);
    }
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_close_obj, 1, i2c_close);

STATIC mp_obj_t i2c_write(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;

    I2C_CHECK_PARAMS(3);
    I2C_NODE_GET();

    mp_buffer_info_t bufinfo = { 0 };
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    char *buf = (char *)bufinfo.buf;

    mp_uint_t write_size = mp_obj_get_int(args[2]);
    if (write_size > bufinfo.len) {
        LOGE(LOG_TAG, "%s: write size[%d] exceed buffer size[%d]\n", __func__, write_size, bufinfo.len);
        return MP_OBJ_NEW_SMALL_INT(-MP_ENFILE);
    }

    ret =
        aos_hal_i2c_mem_write(i2c_device, i2c_device->config.dev_addr, buf[0], 1, &buf[1], write_size - 1, I2C_TIMEOUT);
    if (ret < 0) {
        LOGE(LOG_TAG, "aos_hal_i2c_mem_write failed\n");
        return MP_OBJ_NEW_SMALL_INT(ret);
    } else {
        return MP_OBJ_NEW_SMALL_INT(write_size);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_write_obj, 3, i2c_write);

STATIC mp_obj_t i2c_read(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;

    I2C_CHECK_PARAMS(2);
    I2C_NODE_GET();

    mp_buffer_info_t bufinfo = { 0 };
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    const char *buf = (const char *)bufinfo.buf;

    mp_uint_t read_size = mp_obj_get_int(args[2]);
    if (read_size > bufinfo.len) {
        LOGE(LOG_TAG, "%s: read size[%d] exceed buffer size[%d]\n", __func__, read_size, bufinfo.len);
        return MP_OBJ_NEW_SMALL_INT(-MP_ENFILE);
    }

    ret = aos_hal_i2c_mem_write(i2c_device, i2c_device->config.dev_addr, buf[0], 1, &buf[0], 1, I2C_TIMEOUT);
    if (ret < 0) {
        LOGE(LOG_TAG, "aos_hal_i2c_master_send failed\n");
        return MP_OBJ_NEW_SMALL_INT(ret);
    }

    ret = aos_hal_i2c_mem_read(i2c_device, i2c_device->config.dev_addr, buf[0], 1, &buf[0], read_size, I2C_TIMEOUT_MS);
    if (ret < 0) {
        LOGE(LOG_TAG, "hal_i2c_master_recv failed\n");
        return MP_OBJ_NEW_SMALL_INT(ret);
    } else {
        return MP_OBJ_NEW_SMALL_INT(read_size);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(i2c_read_obj, 2, i2c_read);

STATIC const mp_rom_map_elem_t i2c_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_I2C) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&i2c_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&i2c_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&i2c_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&i2c_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(i2c_locals_dict, i2c_locals_dict_table);

const mp_obj_type_t driver_i2c_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = i2c_obj_print,
    .make_new = i2c_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&i2c_locals_dict,
};
