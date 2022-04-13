/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>

#include "amp_defines.h"
#include "amp_hal_adc.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "ADC"

static duk_ret_t native_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t adc_handle;
    adc_handle.handle     = NULL;
    adc_dev_t *adc_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string");
        goto out;
    }

    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_ADC, id, &adc_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!");
        goto out;
    }

    amp_debug(MOD_STR, "adc handle:%u\n", adc_handle.handle);
    adc_device = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = amp_hal_adc_init(adc_device);

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
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    adc_handle.handle = duk_get_pointer(ctx, 0);
    adc_device        = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = amp_hal_adc_finalize(adc_device);
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
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    adc_handle.handle = duk_get_pointer(ctx, 0);
    adc_device        = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    (void)amp_hal_adc_value_get(adc_device, (void *)&adc_value, 0);
    amp_debug(MOD_STR, "adc value: %d\n", adc_value);
out:
    duk_push_int(ctx, adc_value > 0 ? adc_value : -1);
    return 1;
}

void module_adc_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, native_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_prop_string(ctx, -2, "ADC");
}
