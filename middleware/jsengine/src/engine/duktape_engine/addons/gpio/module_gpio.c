/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <stdint.h>
#include "be_jse_task.h"
#include "hal/log.h"
#include "board-mgr/board_mgr.h"
#include "bone_engine_inl.h"
#include "aos/hal/gpio.h"

#define GPIO_IRQ_RISING_EDGE "rising"
#define GPIO_IRQ_FALLING_EDGE "falling"
#define GPIO_IRQ_BOTH_EDGE "both"

static duk_ret_t native_open(duk_context *ctx)
{
    int8_t ret = -1;
    item_handle_t gpio_handle;
    gpio_handle.handle      = 0xFFFFFFFF;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        jse_warn("parameter must be string\n");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_GPIO, id, &gpio_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }
    jse_debug("gpio handle:%u\n", gpio_handle.handle);
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_gpio_init(gpio_device);
    if (0 != ret) {
        jse_error("hal_gpio_init fail!\n");
        goto out;
    }
    gpio_device->priv = NULL;
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_GPIO, &gpio_handle);
    } else {
        duk_push_pointer(ctx, (void *)gpio_handle.handle);
    }
    return 1;
}

static duk_ret_t native_close(duk_context *ctx)
{
    int8_t result = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    int js_cb_ref = (int)gpio_device->priv;
    bone_engine_unref(ctx, js_cb_ref);
    gpio_device->priv = NULL;
    board_disattach_item(MODULE_GPIO, &gpio_handle);
    result = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

static duk_ret_t native_write(duk_context *ctx)
{
    int8_t ret    = -1;
    int8_t result = -1;
    int8_t level  = 0;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0) || !duk_is_number(ctx, 1)) {
        jse_warn("parameter must be handle and number\n");
        goto out;
    }
    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    level = duk_get_int(ctx, 1);
    if (level) {
        ret = hal_gpio_output_high(gpio_device);
    } else {
        ret = hal_gpio_output_low(gpio_device);
    }
    if (-1 == ret) {
        jse_error("gpio output set fail!\n");
        goto out;
    }
    result = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

static duk_ret_t native_read(duk_context *ctx)
{
    item_handle_t gpio_handle;
    uint32_t level          = 0;
    gpio_dev_t *gpio_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        jse_warn("parameter must be handle\n");
        goto out;
    }
    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    hal_gpio_input_get(gpio_device, &level);
out:
    duk_push_int(ctx, level);
    return 1;
}

struct gpio_irq_notify_param {
    int js_cb_ref;
    int value;
};

static void gpio_irq_notify(void *arg)
{
    struct gpio_irq_notify_param *p = (struct gpio_irq_notify_param *)arg;
    jse_debug("value: 0x%x\n", p->value);
    duk_context *ctx = bone_engine_get_context();
    bone_engine_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->value);
    duk_pcall(ctx, 1);
    duk_pop(ctx);
    jse_free(p);
}

/* 中断中避免调用打印 */
static void gpio_irq(void *arg)
{
    uint32_t value   = 0;
    gpio_dev_t *gpio = (gpio_dev_t *)arg;

    if (NULL == gpio) {
        /* jse_error("param error!\n"); */
        return;
    }

    int js_cb_ref = (int)gpio->priv;
    if (js_cb_ref <= 0) {
        /* jse_error("js cb ref error, ref: %d\n", js_cb_ref); */
        return;
    }

    hal_gpio_input_get(gpio, &value);

    struct gpio_irq_notify_param *p =
        (struct gpio_irq_notify_param *)jse_malloc(sizeof(*p));
    p->js_cb_ref = js_cb_ref;
    p->value     = value;
    if (be_jse_task_schedule_call(gpio_irq_notify, p) < 0) {
        /* jse_warn("be_jse_task_schedule_call failed\n"); */
        jse_free(p);
    }
}

static duk_ret_t native_on(duk_context *ctx)
{
    int8_t ret      = -1;
    int8_t result   = -1;
    int8_t irq_edge = 0;
    item_handle_t gpio_handle;
    gpio_handle.handle      = 0xFFFFFFFF;
    gpio_dev_t *gpio_device = NULL;
    const char *edge;

    if (!duk_is_pointer(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_function(ctx, 2)) {
        jse_warn("parameter must be handle, string and function\n");
        goto out;
    }
    gpio_handle.handle = (uint32_t)duk_get_pointer(ctx, 0);
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    edge = duk_get_string(ctx, 1);
    if (0 == strcmp(GPIO_IRQ_RISING_EDGE, edge)) {
        irq_edge = IRQ_TRIGGER_RISING_EDGE;
    } else if (0 == strcmp(GPIO_IRQ_FALLING_EDGE, edge)) {
        irq_edge = IRQ_TRIGGER_FALLING_EDGE;
    } else if (0 == strcmp(GPIO_IRQ_BOTH_EDGE, edge)) {
        irq_edge = IRQ_TRIGGER_BOTH_EDGES;
    } else {
        jse_error("irq edge wrong!\n");
        goto out;
    }
    jse_debug("irq_edge:%d port:%d\n", irq_edge, gpio_device->port);
    ret = hal_gpio_enable_irq(gpio_device, irq_edge, gpio_irq, gpio_device);
    if (ret < 0) {
        jse_error("hal_gpio_enable_irq fail!\n");
        goto out;
    }
    duk_dup(ctx, 2);
    int js_cb_ref     = bone_engine_ref(ctx);
    gpio_device->priv = (void *)js_cb_ref;
    result            = 0;
out:
    duk_push_int(ctx, result);
    return 1;
}

void module_gpio_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, native_write, 2);
    duk_put_prop_string(ctx, -2, "write");

    duk_push_c_function(ctx, native_on, 3);
    duk_put_prop_string(ctx, -2, "on");

    duk_push_c_function(ctx, native_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "GPIO");
}
