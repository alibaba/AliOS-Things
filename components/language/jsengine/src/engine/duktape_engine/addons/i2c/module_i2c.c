/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "jse_common.h"
#include "be_inl.h"

#define I2C_TIMEOUT (0xFFFFFF)

static duk_ret_t native_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t i2c_handle;
    i2c_handle.handle     = 0xFFFFFFFF;
    i2c_dev_t *i2c_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("parameter must be string\n");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_I2C, id, &i2c_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }
    jse_debug("i2c handle:%u\n", i2c_handle.handle);
    i2c_device = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_i2c_init(i2c_device);
    if (0 != ret) {
        jse_error("hal_i2c_init fail!\n");
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

static duk_ret_t native_close(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    i2c_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_i2c_finalize(i2c_device);
    if (0 != ret) {
        jse_error("hal_i2c_finalize fail!\n");
        goto out;
    }
    board_disattach_item(MODULE_I2C, &i2c_handle);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_write(duk_context *ctx)
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
        jse_warn("parameter must be handle and array\n");
        goto out;
    }

    i2c_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }

    arr_idx = duk_normalize_index(ctx, 1);
    len     = duk_get_length(ctx, arr_idx);
    data    = (uint8_t *)jse_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        jse_warn("allocate memory failed\n");
        goto out;
    }
    for (i = 0; i < len; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_number(ctx, -1)) {
            jse_warn("data is not number, index: %d\n", i);
            duk_pop(ctx);
            goto out;
        }
        data[i] = (uint8_t)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    ret = hal_i2c_master_send(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        jse_error("hal_i2c_master_send fail!\n");
        goto out;
    }
    err = 0;
out:
    jse_free(data);
    duk_push_int(ctx, err);
    return 1;
}

static duk_ret_t native_read(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t i2c_handle;
    i2c_dev_t *i2c_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        jse_warn("parameter must be handle and number\n");
        goto out;
    }
    i2c_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    len  = duk_get_int(ctx, 1);
    data = (uint8_t *)jse_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        jse_error("allocate memory failed\n");
        goto out;
    }
    ret = hal_i2c_master_recv(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        jse_error("hal_i2c_master_recv fail!\n");
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
    jse_free(data);
    return 1;
}

void module_i2c_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_read, 2);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, native_write, 2);
    duk_put_prop_string(ctx, -2, "write");

    duk_push_c_function(ctx, native_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "I2C");
}
