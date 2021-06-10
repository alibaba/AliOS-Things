/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_i2c.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "I2C"

#define I2C_TIMEOUT (0xFFFFFF)

static duk_ret_t native_i2c_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t i2c_handle;
    i2c_handle.handle     = NULL;
    i2c_dev_t *i2c_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_I2C, id, &i2c_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!");
        goto out;
    }
    amp_debug(MOD_STR, "i2c handle:%u", i2c_handle.handle);
    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_i2c_init(i2c_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_i2c_init fail!");
        goto out;
    }
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_I2C, &i2c_handle);
    } else {
        duk_push_pointer(ctx, (void *)i2c_handle.handle);
    }
    return 1;
}

static duk_ret_t native_i2c_close(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    i2c_handle.handle = duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_i2c_finalize(i2c_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_i2c_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_I2C, &i2c_handle);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_i2c_write(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;
    int arr_idx;
    int err = -1;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle and array");
        goto out;
    }

    i2c_handle.handle = duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    arr_idx = duk_normalize_index(ctx, 1);
    len     = duk_get_length(ctx, arr_idx);
    data    = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    for (i = 0; i < len; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_number(ctx, -1)) {
            amp_warn(MOD_STR, "data is not number, index is %d", i);
            duk_pop(ctx);
            goto out;
        }
        data[i] = (uint8_t)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    ret = aos_hal_i2c_master_send(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_i2c_master_send fail!");
        goto out;
    }
    err = 0;
out:
    aos_free(data);
    duk_push_int(ctx, err);
    return 1;
}

static duk_ret_t native_i2c_write_reg(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;
    uint16_t mem_addr;
    int arr_idx;
    int err = -1;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1) || !duk_is_array(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be handle number and array");
        goto out;
    }

    i2c_handle.handle = duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    mem_addr = (uint16_t)duk_get_int(ctx, 1);

    arr_idx = duk_normalize_index(ctx, 2);
    len     = duk_get_length(ctx, arr_idx);
    data    = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    for (i = 0; i < len; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_number(ctx, -1)) {
            amp_warn(MOD_STR, "data is not number, index is %d", i);
            duk_pop(ctx);
            goto out;
        }
        data[i] = (uint8_t)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    ret = aos_hal_i2c_mem_write(i2c_device, i2c_device->config.dev_addr,
                                mem_addr, 1, data, len, I2C_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_i2c_master_send fail!");
        goto out;
    }
    err = 0;
out:
    aos_free(data);
    duk_push_int(ctx, err);
    return 1;
}

static duk_ret_t native_i2c_read(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle and number");
        goto out;
    }
    i2c_handle.handle = duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    len  = duk_get_int(ctx, 1);
    data = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }
    ret = aos_hal_i2c_master_recv(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "aos_hal_i2c_master_recv fail!");
    }
out:
    if (!ret) {
        duk_idx_t arr_idx = duk_push_array(ctx);
        for (i = 0; i < len; i++) {
            duk_push_int(ctx, data[i]);
            duk_put_prop_index(ctx, arr_idx, i);
        }
    } else {
        duk_push_null(ctx);
    }
    aos_free(data);
    return 1;
}

static duk_ret_t native_i2c_read_reg(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;
    uint16_t mem_addr;
    int arr_idx;
    int err = -1;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1) || !duk_is_number(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be handle number and array");
        goto out;
    }

    i2c_handle.handle = duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    mem_addr = (uint16_t)duk_get_int(ctx, 1);

    len     = duk_get_int(ctx, 2);
    data    = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    ret = aos_hal_i2c_mem_read(i2c_device, i2c_device->config.dev_addr,
                                mem_addr, 1, data, len, I2C_TIMEOUT);

out:
    if (!ret) {
        duk_idx_t arr_idx = duk_push_array(ctx);
        for (i = 0; i < len; i++) {
            duk_push_int(ctx, data[i]);
            duk_put_prop_index(ctx, arr_idx, i);
        }
    } else {
        duk_push_null(ctx);
    }
    aos_free(data);
    return 1;
}

void module_i2c_register(void)
{
    amp_debug(MOD_STR, "module_i2c_register");
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("open",     native_i2c_open, 1);
    AMP_ADD_FUNCTION("read",     native_i2c_read, 2);
    AMP_ADD_FUNCTION("write",    native_i2c_write, 2);
    AMP_ADD_FUNCTION("readReg",  native_i2c_read_reg, 3);
    AMP_ADD_FUNCTION("writeReg", native_i2c_write_reg, 3);
    AMP_ADD_FUNCTION("close",    native_i2c_close, 1);

    duk_put_prop_string(ctx, -2, "I2C");
}
