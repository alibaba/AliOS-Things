/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>
#include "aos/hal/adc.h"
#include "hal/log.h"
#include "board-mgr/board_mgr.h"
#include "bone_engine_inl.h"

static duk_ret_t native_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t adc_handle;
    adc_handle.handle     = 0xFFFFFFFF;
    adc_dev_t *adc_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("parameter must be string\n");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_ADC, id, &adc_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }
    jse_debug("adc handle:%u\n", adc_handle.handle);
    adc_device = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_adc_init(adc_device);
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_ADC, &adc_handle);
    } else {
        duk_push_pointer(ctx, (void *)adc_handle.handle);
    }
    return 1;
}

static duk_ret_t native_close(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t adc_handle;
    adc_dev_t *adc_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    adc_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    adc_device        = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_adc_finalize(adc_device);
    board_disattach_item(MODULE_ADC, &adc_handle);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_read(duk_context *ctx)
{
    int32_t adc_value = -1;
    item_handle_t adc_handle;
    adc_dev_t *adc_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    adc_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    adc_device        = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    (void)hal_adc_value_get(adc_device, (void *)&adc_value, 0);
    jse_debug("adc value: %d\n", adc_value);
out:
    duk_push_int(ctx, adc_value > 0 ? adc_value : -1);
    return 1;
}

void module_adc_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, native_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "ADC");
}
