/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_jse_module.h"
#include "be_port_hal.h"
#include "board-mgr/board_mgr.h"

#define I2C_TIMEOUT (0xFFFFFF)

static be_jse_symbol_t *i2c_open(void)
{
    int32_t len   = -1;
    char *data    = NULL;
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t i2c_handle;
    i2c_handle.handle     = 0xFFFFFFFF;
    be_jse_symbol_t *arg0 = NULL;
    i2c_dev_t *i2c_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_string(arg0)) {
        goto out;
    }
    len  = symbol_str_len(arg0);
    data = jse_calloc(1, sizeof(char) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    symbol_to_str(arg0, data, len);
    ret = board_attach_item(MODULE_I2C, data, &i2c_handle);
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
    result = 0;
out:

    if (NULL != data) {
        jse_free(data);
        data = NULL;
    }
    symbol_unlock(arg0);
    if (0 != result) {
        board_disattach_item(MODULE_I2C, &i2c_handle);
        return new_int_symbol(-1);
    }

    return new_int_symbol(i2c_handle.handle);
}

static be_jse_symbol_t *i2c_close(void)
{
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t i2c_handle;
    be_jse_symbol_t *arg0 = NULL;
    i2c_dev_t *i2c_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    i2c_handle.handle = get_symbol_value_int(arg0);
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
    result = 0;
out:
    symbol_unlock(arg0);

    return new_int_symbol(result);
}

static be_jse_symbol_t *i2c_write(void)
{
    int8_t ret    = -1;
    int8_t result = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t i2c_handle;
    be_jse_symbol_t *item = NULL;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    i2c_dev_t *i2c_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    i2c_handle.handle = get_symbol_value_int(arg0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    if (!arg1 || !symbol_is_array(arg1)) {
        goto out;
    }
    len  = get_symbol_array_length(arg1);
    data = jse_calloc(1, sizeof(uint8_t) * (len + 1));
    if (NULL == data) {
        goto out;
    }

    for (i = 0; i < len; ++i) {
        item = get_symbol_array_item(arg1, i);
        if (!item || !symbol_is_int(item)) {
            goto out;
        }
        data[i] = (uint8_t)get_symbol_value_int(item);
        symbol_unlock(item);
    }
    ret = hal_i2c_master_send(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        jse_error("hal_i2c_master_send fail!\n");
        goto out;
    }
    result = 0;
out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    if (NULL != data) {
        jse_free(data);
        data = NULL;
    }

    return new_int_symbol(len);
}

static be_jse_symbol_t *i2c_read(void)
{
    int8_t ret    = -1;
    int8_t result = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t i2c_handle;
    be_jse_symbol_t *arr  = NULL;
    be_jse_symbol_t *item = NULL;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    i2c_dev_t *i2c_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    i2c_handle.handle = get_symbol_value_int(arg0);
    i2c_device        = board_get_node_by_handle(MODULE_I2C, &i2c_handle);
    if (NULL == i2c_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    if (!arg1 || !symbol_is_int(arg1)) {
        goto out;
    }
    len  = get_symbol_value_int(arg1);
    data = jse_calloc(1, sizeof(uint8_t) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    ret = hal_i2c_master_recv(i2c_device, i2c_device->config.dev_addr, data,
                              len, I2C_TIMEOUT);
    if (-1 == ret) {
        jse_error("hal_i2c_master_recv fail!\n");
        goto out;
    }
    arr = new_symbol(BE_SYM_ARRAY);
    for (i = 0; i < len; ++i) {
        be_jse_symbol_t *idx = new_int_symbol(data[i]);
        symbol_array_push(arr, idx);
        symbol_unlock(idx);
    }
    result = 0;
out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    if (NULL != data) {
        jse_free(data);
        data = NULL;
    }

    return (0 == result) ? arr : new_symbol(BE_SYM_NULL);
}

static be_jse_symbol_t *i2c_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name)
{
    if (0 == strcmp(name, "open")) {
        return i2c_open();
    }
    if (0 == strcmp(name, "read")) {
        return i2c_read();
    }
    if (0 == strcmp(name, "write")) {
        return i2c_write();
    }
    if (0 == strcmp(name, "close")) {
        return i2c_close();
    }
    return (BE_JSE_FUNC_UNHANDLED);
}

void module_i2c_register(void)
{
    be_jse_module_load("I2C", i2c_module_handle_cb);
}
