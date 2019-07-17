/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include "module_pwm.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_jse_module.h"
#include "be_port_hal.h"
#include "board-mgr/board_mgr.h"

static be_jse_symbol_t *pwm_start(void) {
    int32_t len   = -1;
    char *data    = NULL;
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t pwm_handle;
    pwm_handle.handle     = 0xFFFFFFFF;
    be_jse_symbol_t *arg0 = NULL;
    pwm_dev_t *pwm_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_string(arg0)) {
        goto out;
    }
    len  = symbol_str_len(arg0);
    data = calloc(1, sizeof(char) * (len + 1));
    if (NULL == data) goto out;
    symbol_to_str(arg0, data, len);
    ret = board_attach_item(MODULE_PWM, data, &pwm_handle);
    if (0 != ret) {
        be_error("pwm", "board_attach_item fail!\n");
        goto out;
    }
    be_debug("pwm", "gpio handle:%u\n", pwm_handle.handle);
    pwm_device = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        be_error("pwm", "board_get_node_by_handle fail!\n");
        goto out;
    }
    be_debug("pwm", "%s:%d:%d:%f\n", data, pwm_device->port,
             pwm_device->config.freq, pwm_device->config.duty_cycle);
    hal_pwm_stop(pwm_device);
    hal_pwm_init(pwm_device);
    hal_pwm_start(pwm_device);
    result = 0;

out:

    if (NULL != data) {
        free(data);
        data = NULL;
    }
    symbol_unlock(arg0);
    if (0 != result) {
        board_disattach_item(MODULE_PWM, &pwm_handle);
        return new_int_symbol(-1);
    }
    return new_int_symbol(pwm_handle.handle);
}

static be_jse_symbol_t *pwm_stop(void) {
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t pwm_handle;
    be_jse_symbol_t *arg0 = NULL;
    pwm_dev_t *pwm_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    pwm_handle.handle = get_symbol_value_int(arg0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        be_error("pwm", "board_get_node_by_handle fail!\n");
        goto out;
    }
    hal_pwm_stop(pwm_device);
    ret = hal_pwm_finalize(pwm_device);
    board_disattach_item(MODULE_PWM, &pwm_handle);
    result = 0;
out:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *pwm_get_duty(void) {
    int32_t ret = -1;
    item_handle_t pwm_handle;
    be_jse_symbol_t *arg0 = NULL;
    pwm_dev_t *pwm_device = NULL;
    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    pwm_handle.handle = get_symbol_value_int(arg0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        be_error("pwm", "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = (int)(pwm_device->config.duty_cycle * 100);
out:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *pwm_set_duty(void) {
    int8_t ret    = -1;
    int8_t result = -1;
    uint32_t duty = 0;
    item_handle_t pwm_handle;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    pwm_dev_t *pwm_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    pwm_handle.handle = get_symbol_value_int(arg0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        be_error("pwm", "board_get_node_by_handle fail!\n");
        goto out;
    }
    if (!arg1 || !symbol_is_int(arg1)) {
        goto out;
    }
    duty = get_symbol_value_int(arg1);
    duty = (duty > 100) ? (100) : (duty);
    hal_pwm_stop(pwm_device);
    pwm_device->config.duty_cycle = (duty / 100.0);
    hal_pwm_init(pwm_device);
    hal_pwm_start(pwm_device);
    result = 0;
out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(result);
}

static be_jse_symbol_t *pwm_get_freq(void) {
    int32_t ret = -1;
    item_handle_t pwm_handle;
    be_jse_symbol_t *arg0 = NULL;
    pwm_dev_t *pwm_device = NULL;
    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    pwm_handle.handle = get_symbol_value_int(arg0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        be_error("pwm", "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = (int)(pwm_device->config.freq);
out:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *pwm_set_freq(void) {
    int8_t ret    = -1;
    int8_t result = -1;
    uint32_t freq = 0;
    item_handle_t pwm_handle;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    pwm_dev_t *pwm_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    pwm_handle.handle = get_symbol_value_int(arg0);
    pwm_device        = board_get_node_by_handle(MODULE_PWM, &pwm_handle);
    if (NULL == pwm_device) {
        be_error("pwm", "board_get_node_by_handle fail!\n");
        goto out;
    }
    if (!arg1 || !symbol_is_int(arg1)) {
        goto out;
    }
    freq = get_symbol_value_int(arg1);
    hal_pwm_stop(pwm_device);
    pwm_device->config.freq = freq;
    hal_pwm_init(pwm_device);
    hal_pwm_start(pwm_device);
    result = 0;
out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(result);
}

static be_jse_symbol_t *pwm_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name) {
    if (0 == strcmp(name, "start")) {
        return pwm_start();
    }
    if (0 == strcmp(name, "stop")) {
        return pwm_stop();
    }
    if (0 == strcmp(name, "getDuty")) {
        return pwm_get_duty();
    }
    if (0 == strcmp(name, "setDuty")) {
        return pwm_set_duty();
    }
    if (0 == strcmp(name, "getFreq")) {
        return pwm_get_freq();
    }
    if (0 == strcmp(name, "setFreq")) {
        return pwm_set_freq();
    }
    return (BE_JSE_FUNC_UNHANDLED);
}

void module_pwm_register(void) {
    be_jse_module_load("PWM", pwm_module_handle_cb);
}
