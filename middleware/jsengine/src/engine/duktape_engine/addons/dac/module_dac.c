/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>
#include "hal/log.h"
#include "board-mgr/board_mgr.h"
#include "bone_engine_inl.h"
#include "aos/hal/dac.h"

static duk_ret_t native_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t dac_handle;
    dac_handle.handle     = 0xFFFFFFFF;
    dac_dev_t *dac_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("parameter must be string\n");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_DAC, id, &dac_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }
    jse_debug("dac handle:%u\n", dac_handle.handle);
    dac_device = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_dac_init(dac_device);
    if (0 != ret) {
        jse_error("hal_dac_init fail!\n");
        goto out;
    }
    ret = hal_dac_start(dac_device, dac_device->port);
    if (0 != ret) {
        jse_error("hal_dac_start fail!\n");
    }
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_DAC, &dac_handle);
    } else {
        duk_push_pointer(ctx, (void *)dac_handle.handle);
    }
    return 1;
}

static duk_ret_t native_setVol(duk_context *ctx)
{
    int8_t ret       = -1;
    uint32_t voltage = 0;
    item_handle_t dac_handle;
    dac_dev_t *dac_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        jse_warn("parameter must be handle and number\n");
        goto out;
    }
    dac_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    dac_device        = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    voltage = duk_get_int(ctx, 1);
    ret     = hal_dac_set_value(dac_device, dac_device->port, voltage);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_getVol(duk_context *ctx)
{
    int32_t ret = -1;
    item_handle_t dac_handle;
    dac_dev_t *dac_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    dac_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    dac_device        = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = (int)hal_dac_get_value(dac_device, dac_device->port);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_close(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t dac_handle;
    dac_dev_t *dac_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    dac_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    dac_device        = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    hal_dac_stop(dac_device, dac_device->port);
    ret = hal_dac_finalize(dac_device);
    board_disattach_item(MODULE_DAC, &dac_handle);
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_dac_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_getVol, 1);
    duk_put_prop_string(ctx, -2, "getVol");

    duk_push_c_function(ctx, native_setVol, 2);
    duk_put_prop_string(ctx, -2, "setVol");

    duk_push_c_function(ctx, native_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "DAC");
}
