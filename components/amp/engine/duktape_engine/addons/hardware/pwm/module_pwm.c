/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_pwm.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "PWM"

typedef struct sim_info {
    int freq;
    int duty;
} pwm_module_t;

static duk_ret_t native_pwm_open(duk_context *ctx)
{
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t pwm_handle;
    pwm_handle.handle     = NULL;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_PWM, id, &pwm_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!\n");
        goto out;
    }
    amp_debug(MOD_STR, "gpio handle:%u\n", pwm_handle.handle);
    pwm_device = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    amp_debug(MOD_STR, "%s:%d:%d:%f\n", id, pwm_device->port, pwm_device->config.freq,
          pwm_device->config.duty_cycle);

    ret = aos_hal_pwm_init(pwm_device);

out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_PWM, &pwm_handle);
    } else {
        duk_push_pointer(ctx, (void *)pwm_handle.handle);
    }
    return 1;
}

static duk_ret_t native_pwm_close(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    pwm_handle.handle = duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = aos_hal_pwm_stop(pwm_device);
    ret |= aos_hal_pwm_finalize(pwm_device);
    board_disattach_item(MODULE_PWM, &pwm_handle);
    amp_debug(MOD_STR, "aos_hal_pwm_finalize ret: %d\n", ret);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pwm_getConfig(duk_context *ctx)
{
    int32_t ret = -1;
    item_handle_t pwm_handle;
    pwm_module_t pwm_config;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    pwm_handle.handle = duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    pwm_config.duty = (int)(pwm_device->config.duty_cycle * 100);
    pwm_config.freq = (int)(pwm_device->config.freq);

    duk_push_object(ctx);

    duk_push_int(ctx, pwm_config.freq);
    duk_put_prop_string(ctx, -2, "freq");

    duk_push_int(ctx, pwm_config.duty);
    duk_put_prop_string(ctx, -2, "duty");

    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_pwm_setConfig(duk_context *ctx)
{
    int8_t ret    = -1;
    int32_t freq;
    int duty    = 0;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_object(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle and number");
        goto out;
    }
    pwm_handle.handle = duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!\n");
        goto out;
    }

    duk_get_prop_string(ctx, 1, "freq");
    duk_get_prop_string(ctx, 1, "duty");

    freq = duk_get_number(ctx, -2);
    duty = duk_get_number(ctx, -1);

    pwm_device->config.duty_cycle = (float)duty / 100.0;
    pwm_device->config.freq = freq;

    ret = aos_hal_pwm_stop(pwm_device);
    if (ret != 0) {
        amp_warn(MOD_STR, "amp hal pwm stop failed\n");
        goto out;
    }
    ret = aos_hal_pwm_init(pwm_device);
    if (ret != 0) {
        amp_warn(MOD_STR, "amp hal pwm init failed\n");
        goto out;
    }
    ret = aos_hal_pwm_start(pwm_device);

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_pwm_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("open",      native_pwm_open, 1);
    AMP_ADD_FUNCTION("close",     native_pwm_close, 1);
    AMP_ADD_FUNCTION("getConfig", native_pwm_getConfig, 1);
    AMP_ADD_FUNCTION("setConfig", native_pwm_setConfig, 2);

    duk_put_prop_string(ctx, -2, "PWM");
}
