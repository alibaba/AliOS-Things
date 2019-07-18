/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>
#include "be_log.h"
#include "board-mgr/board_mgr.h"
#include "bone_engine_inl.h"
#include "pwm.h"

static duk_ret_t native_start(duk_context *ctx)
{
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t pwm_handle;
    pwm_handle.handle     = 0xFFFFFFFF;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        warn("parameter must be string\n");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_PWM, id, &pwm_handle);
    if (0 != ret) {
        error("board_attach_item fail!\n");
        goto out;
    }
    debug("gpio handle:%u\n", pwm_handle.handle);
    pwm_device = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        error("board_get_node_by_handle fail!\n");
        goto out;
    }
    debug("%s:%d:%d:%f\n", id, pwm_device->port, pwm_device->config.freq,
          pwm_device->config.duty_cycle);
    hal_pwm_init(pwm_device);
    result = 0;

out:
    if (0 != result) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_PWM, &pwm_handle);
    } else {
        duk_push_pointer(ctx, (void *)pwm_handle.handle);
    }
    return 1;
}

static duk_ret_t native_stop(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    debug("in\n");
    if (!duk_is_pointer(ctx, 0)) {
        warn("parameter must be handle\n");
        goto out;
    }
    pwm_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_pwm_stop(pwm_device);
    ret |= hal_pwm_finalize(pwm_device);
    board_disattach_item(MODULE_PWM, &pwm_handle);
    debug("hal_pwm_finalize ret: %d\n", ret);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_getDuty(duk_context *ctx)
{
    int32_t ret = -1;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        warn("parameter must be handle\n");
        goto out;
    }

    pwm_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = (int)(pwm_device->config.duty_cycle * 100);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_setDuty(duk_context *ctx)
{
    int8_t ret    = -1;
    int8_t result = -1;
    float duty    = 0;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        warn("parameter must be handle and number\n");
        goto out;
    }
    pwm_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        error("board_get_node_by_handle fail!\n");
        goto out;
    }
    duty                          = (float)duk_get_number(ctx, 1);
    pwm_device->config.duty_cycle = duty / 100.0;
    hal_pwm_start(pwm_device);
    result = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

static duk_ret_t native_getFreq(duk_context *ctx)
{
    int32_t ret = -1;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        warn("parameter must be handle\n");
        goto out;
    }
    pwm_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = (int)(pwm_device->config.freq);
out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_setFreq(duk_context *ctx)
{
    int8_t ret    = -1;
    int8_t result = -1;
    uint32_t freq = 0;
    item_handle_t pwm_handle;
    pwm_dev_t *pwm_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        warn("parameter must be handle and number\n");
        goto out;
    }
    pwm_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        error("board_get_node_by_handle fail!\n");
        goto out;
    }
    freq = duk_get_int(ctx, 1);
    hal_pwm_stop(pwm_device);
    pwm_device->config.freq = freq;
    hal_pwm_init(pwm_device);
    hal_pwm_start(pwm_device);
    result = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

void module_pwm_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_start, 1);
    duk_put_prop_string(ctx, -2, "start");

    duk_push_c_function(ctx, native_stop, 1);
    duk_put_prop_string(ctx, -2, "stop");

    duk_push_c_function(ctx, native_getDuty, 1);
    duk_put_prop_string(ctx, -2, "getDuty");

    duk_push_c_function(ctx, native_setDuty, 2);
    duk_put_prop_string(ctx, -2, "setDuty");

    duk_push_c_function(ctx, native_getFreq, 1);
    duk_put_prop_string(ctx, -2, "getFreq");

    duk_push_c_function(ctx, native_setFreq, 2);
    duk_put_prop_string(ctx, -2, "setFreq");

    duk_put_global_string(ctx, "PWM");
}
