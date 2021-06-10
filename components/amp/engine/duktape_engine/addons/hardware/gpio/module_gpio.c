/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_gpio.h"
#include "aos_system.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "GPIO"

#define GPIO_IRQ_RISING_EDGE "rising"
#define GPIO_IRQ_FALLING_EDGE "falling"
#define GPIO_IRQ_BOTH_EDGE "both"

static uint16_t gpio_init_flag = 0;
static duk_ret_t native_gpio_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t gpio_handle;
    gpio_handle.handle      = NULL;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret = board_attach_item(MODULE_GPIO, id, &gpio_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!, id %s", id);
        goto out;
    }
    amp_debug(MOD_STR, "gpio handle:%p\n", gpio_handle.handle);
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    if (gpio_init_flag & (1 << gpio_device->port)) {
        amp_debug(MOD_STR, "gpio port [%d] is already inited", gpio_device->port);
        goto out;
    }
    ret = aos_hal_gpio_init(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_init fail!");
        goto out;
    }
    // gpio_device->priv = NULL;
    gpio_init_flag |= (1 << gpio_device->port);

out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_GPIO, &gpio_handle);
    } else {
        duk_push_pointer(ctx, (void *)gpio_handle.handle);
    }
    return 1;
}

static duk_ret_t native_gpio_close(duk_context *ctx)
{
    int32_t ret = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    gpio_handle.handle = duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    ret = aos_hal_gpio_finalize(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_GPIO, &gpio_handle);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_gpio_toggle(duk_context *ctx)
{
    int32_t ret = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    gpio_handle.handle = duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    ret = aos_hal_gpio_output_toggle(gpio_device);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_gpio_write(duk_context *ctx)
{
    int8_t ret    = -1;
    int8_t result = -1;
    int8_t level  = 0;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle and number");
        goto out;
    }
    gpio_handle.handle = duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    level = duk_get_int(ctx, 1);
    if (level) {
        ret = aos_hal_gpio_output_high(gpio_device);
    } else {
        ret = aos_hal_gpio_output_low(gpio_device);
    }
    if (-1 == ret) {
        amp_error(MOD_STR, "gpio output set fail!");
        goto out;
    }
    result = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

static duk_ret_t native_gpio_read(duk_context *ctx)
{
    item_handle_t gpio_handle;
    uint32_t level          = 0;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    gpio_handle.handle = duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    aos_hal_gpio_input_get(gpio_device, &level);
out:
    duk_push_int(ctx, level);
    return 1;
}

typedef struct {
    int js_cb_ref;
    gpio_dev_t *dev;
}gpio_irq_notify_param_t;

static void gpio_irq_notify(void *arg)
{
    gpio_irq_notify_param_t *param = (gpio_irq_notify_param_t *)arg;
    duk_context *ctx = be_get_context();
    uint32_t value = 0;
    aos_hal_gpio_input_get(param->dev, &value);
    be_push_ref(ctx, param->js_cb_ref);
    duk_push_int(ctx, value);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    aos_free(param);
}

/* avoid stdout in irq function */
static void gpio_irq(void *arg)
{
    static uint64_t irq_lasttime = 0;
    uint64_t irq_nowtime = aos_now_ms();
    gpio_dev_t *gpio = (gpio_dev_t *)arg;
    gpio_params_t *priv = NULL;
    gpio_irq_notify_param_t *notify;

    if (NULL == gpio) {
        /* amp_error(MOD_STR, "param error!\n"); */
        return;
    }

    priv = (gpio_params_t *)gpio->priv;
    int js_cb_ref = (int)priv->js_cb_ref;
    if (js_cb_ref <= 0) {
        /* amp_error(MOD_STR, "js cb ref error, ref: %d\n", js_cb_ref); */
        return;
    }

    if(irq_nowtime - irq_lasttime < 200){
        // demounce in 200ms
        return;
    }
    irq_lasttime = irq_nowtime;

    notify = aos_malloc(sizeof(gpio_irq_notify_param_t));
    if (!notify)
        return;

    notify->js_cb_ref = js_cb_ref;
    notify->dev = gpio;
    if (amp_task_schedule_call(gpio_irq_notify, notify) < 0) {
        /* amp_warn(MOD_STR, "amp_task_schedule_call failed\n"); */
    }
}

static duk_ret_t native_gpio_on(duk_context *ctx)
{
    int8_t ret      = -1;
    int8_t result   = -1;
    int8_t irq_edge = 0;
    item_handle_t gpio_handle;
    gpio_handle.handle      = NULL;
    gpio_dev_t *gpio_device = NULL;
    gpio_params_t *priv = NULL;
    const char *edge;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be handle, string and function");
        goto out;
    }
    gpio_handle.handle = duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    // edge = duk_get_string(ctx, 1);
    // if (0 == strcmp(GPIO_IRQ_RISING_EDGE, edge)) {
    //     irq_edge = IRQ_TRIGGER_RISING_EDGE;
    // } else if (0 == strcmp(GPIO_IRQ_FALLING_EDGE, edge)) {
    //     irq_edge = IRQ_TRIGGER_FALLING_EDGE;
    // } else if (0 == strcmp(GPIO_IRQ_BOTH_EDGE, edge)) {
    //     irq_edge = IRQ_TRIGGER_BOTH_EDGES;
    // } else {
    //     amp_error(MOD_STR, "irq edge wrong!");
    //     goto out;
    // }
    priv = (gpio_params_t *)gpio_device->priv;
    irq_edge = priv->irq_mode;
    // amp_debug(MOD_STR, "%p, irq_edge:%04x port:%d", gpio_device, irq_edge, gpio_device->port);
    ret = aos_hal_gpio_enable_irq(gpio_device, irq_edge, gpio_irq, gpio_device);
    if (ret < 0) {
        amp_error(MOD_STR, "aos_hal_gpio_enable_irq fail!");
        goto out;
    }
    duk_dup(ctx, 1);
    int js_cb_ref     = be_ref(ctx);
    priv->js_cb_ref = (void *)js_cb_ref;
    result            = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

void module_gpio_register(void)
{
    amp_debug(MOD_STR, "module_gpio_register");
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("open",   native_gpio_open, 1);
    AMP_ADD_FUNCTION("read",   native_gpio_read, 1);
    AMP_ADD_FUNCTION("write",  native_gpio_write, 2);
    AMP_ADD_FUNCTION("toggle", native_gpio_toggle, 1);
    AMP_ADD_FUNCTION("on",     native_gpio_on, 2);
    AMP_ADD_FUNCTION("close",  native_gpio_close, 1);

    duk_put_prop_string(ctx, -2, "GPIO");
}
