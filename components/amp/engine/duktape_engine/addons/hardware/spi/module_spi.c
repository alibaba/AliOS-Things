/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_spi.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "SPI"
#define SPI_TIMEOUT (0xFFFFFF)

static duk_ret_t native_spi_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t spi_handle;
    spi_handle.handle     = NULL;
    spi_dev_t *spi_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_SPI, id, &spi_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!");
        goto out;
    }
    amp_debug(MOD_STR, "spi handle:%u", spi_handle.handle);
    spi_device = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_spi_init(spi_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_spi_init fail!");
        goto out;
    }
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_SPI, &spi_handle);
    } else {
        duk_push_pointer(ctx, (void *)spi_handle.handle);
    }
    return 1;
}

static duk_ret_t native_spi_close(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    spi_handle.handle = duk_get_pointer(ctx, 0);
    spi_device        = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_spi_finalize(spi_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_spi_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_SPI, &spi_handle);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_spi_write(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;
    int arr_idx;
    int err = -1;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle and array");
        goto out;
    }

    spi_handle.handle = duk_get_pointer(ctx, 0);
    spi_device        = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
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
            amp_warn(MOD_STR, "data is not number, index: %d", i);
            duk_pop(ctx);
            goto out;
        }
        data[i] = (uint8_t)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    ret = aos_hal_spi_send(spi_device, data, len, SPI_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "amp_hal_spi_master_send fail!");
        goto out;
    }
    err = 0;
out:
    aos_free(data);
    duk_push_int(ctx, err);
    return 1;
}

static duk_ret_t native_spi_read(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *data = NULL;
    uint32_t len  = 0;
    uint32_t i    = 0;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle and number");
        goto out;
    }
    spi_handle.handle = duk_get_pointer(ctx, 0);
    spi_device        = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    len  = duk_get_int(ctx, 1);
    data = (uint8_t *)aos_malloc(sizeof(uint8_t) * len);
    if (NULL == data) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }
    ret = aos_hal_spi_recv(spi_device, data, len, SPI_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "amp_hal_spi_master_recv fail!");
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

static duk_ret_t native_spi_send_receive(duk_context *ctx)
{
    int8_t ret    = -1;
    uint8_t *send_data = NULL;
    uint8_t *receive_data = NULL;
    uint32_t send_len     = 0;
    uint32_t receive_len  = 0;
    uint32_t i    = 0;
    int arr_idx;
    item_handle_t spi_handle;
    spi_dev_t *spi_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_number(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be handle array and number");
        goto out;
    }
    spi_handle.handle = duk_get_pointer(ctx, 0);
    spi_device        = board_get_node_by_handle(MODULE_SPI, &spi_handle);
    if (NULL == spi_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    arr_idx       = duk_normalize_index(ctx, 1);
    send_len     = duk_get_length(ctx, arr_idx);
    send_data    = (uint8_t *)aos_malloc(sizeof(uint8_t) * send_len);
    if (NULL == send_data) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    for (i = 0; i < send_len; i++) {
        duk_get_prop_index(ctx, arr_idx, i);
        if (!duk_is_number(ctx, -1)) {
            amp_warn(MOD_STR, "data is not number, index: %d", i);
            duk_pop(ctx);
            goto out;
        }
        send_data[i] = (uint8_t)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }

    receive_len  = duk_get_int(ctx, 2);
    receive_data = (uint8_t *)aos_malloc(sizeof(uint8_t) * receive_len);
    if (NULL == receive_data) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }

    ret = aos_hal_spi_send_recv(spi_device, send_data, receive_data, receive_len, SPI_TIMEOUT);
    if (-1 == ret) {
        amp_error(MOD_STR, "amp_hal_spi_master_recv fail!");
    }

out:
    if (!ret) {
        duk_idx_t arr_idx = duk_push_array(ctx);
        for (i = 0; i < receive_len; i++) {
            duk_push_int(ctx, receive_data[i]);
            duk_put_prop_index(ctx, arr_idx, i);
        }
    } else {
        duk_push_null(ctx);
    }

    if (send_data) {
        aos_free(send_data);
    }

    if (receive_data) {
        aos_free(receive_data);
    }

    return 1;
}

void module_spi_register(void)
{
    amp_debug(MOD_STR, "module_spi_register");
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("open",     native_spi_open, 1);
    AMP_ADD_FUNCTION("read",     native_spi_read, 2);
    AMP_ADD_FUNCTION("write",    native_spi_write, 2);
    AMP_ADD_FUNCTION("sendRecv", native_spi_send_receive, 3);
    AMP_ADD_FUNCTION("close",    native_spi_close, 1);

    duk_put_prop_string(ctx, -2, "SPI");
}
