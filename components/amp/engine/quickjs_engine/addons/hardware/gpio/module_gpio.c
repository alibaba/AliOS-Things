/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_hal_gpio.h"
#include "aos_system.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "GPIO"

#define GPIO_IRQ_RISING_EDGE "rising"
#define GPIO_IRQ_FALLING_EDGE "falling"
#define GPIO_IRQ_BOTH_EDGE "both"

static uint16_t gpio_init_flag = 0;
static JSClassID js_gpio_class_id;

static JSValue native_gpio_open(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret = -1;
    item_handle_t gpio_handle;
    gpio_handle.handle      = NULL;
    gpio_dev_t *gpio_device = NULL;

    const char *id = JS_ToCString(ctx, argv[0]);
    if (id == NULL) {
        amp_error(MOD_STR, "get gpio id fail!");
        goto out;
    }

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

    gpio_init_flag |= (1 << gpio_device->port);

out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
    }
    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_GPIO, &gpio_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_gpio_class_id);
        JS_SetOpaque(obj, (void *)gpio_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, ret);
}

static JSValue native_gpio_close(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;
    gpio_params_t *priv = NULL;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_gpio_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    priv = (gpio_params_t *)gpio_device->priv;
    if(priv->reserved != NULL) {
        aos_printf("func %p free memory %p \n", __func__, priv->reserved);
        aos_free(priv->reserved);
        priv->reserved = NULL;
    }
    ret = aos_hal_gpio_finalize(gpio_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_gpio_finalize fail!");
        goto out;
    }
    board_disattach_item(MODULE_GPIO, &gpio_handle);

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_gpio_toggle(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_gpio_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }
    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    ret = aos_hal_gpio_output_toggle(gpio_device);

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_gpio_write(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret    = -1;
    int8_t result = -1;
    int8_t level  = 0;
    item_handle_t gpio_handle;
    gpio_dev_t *gpio_device = NULL;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_gpio_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    JS_ToInt32(ctx, &level, argv[0]);
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
    return JS_NewInt32(ctx, result);
}

static JSValue native_gpio_read(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    item_handle_t gpio_handle;
    uint32_t level          = 0;
    gpio_dev_t *gpio_device = NULL;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_gpio_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    gpio_device = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }
    aos_hal_gpio_input_get(gpio_device, &level);
out:
    return JS_NewInt32(ctx, level);
}

typedef struct {
    JSValue js_cb_ref;
    gpio_dev_t *dev;
}gpio_irq_notify_param_t;

static void gpio_irq_notify(void *arg)
{
    gpio_irq_notify_param_t *param = (gpio_irq_notify_param_t *)arg;
    JSContext *ctx = js_get_context();
    uint32_t value = 0;

    aos_hal_gpio_input_get(param->dev, &value);
    JSValue v = JS_NewInt32(ctx, value);
    JSValue val = JS_Call(ctx, param->js_cb_ref, JS_UNDEFINED, 1, &v);

    JS_FreeValue(ctx, v);
    JS_FreeValue(ctx, val);

    aos_free(param);
}

/* avoid stdout in irq function */
static void gpio_irq(void *arg)
{
    static uint64_t irq_lasttime = 0;
    uint64_t irq_nowtime = aos_now_ms();
    gpio_irq_notify_param_t *notify = aos_malloc(sizeof(gpio_irq_notify_param_t));

    if ((NULL == notify) || (NULL == arg)) {
        /* amp_error(MOD_STR, "param error!\n"); */
        return;
    }

    memcpy(notify, (gpio_irq_notify_param_t *)arg, sizeof(gpio_irq_notify_param_t));
    if(irq_nowtime - irq_lasttime < 200) {
        // demounce in 200ms
        return;
    }
    irq_lasttime = irq_nowtime;
    if (amp_task_schedule_call(gpio_irq_notify, notify) < 0) {
        /* amp_warn(MOD_STR, "amp_task_schedule_call failed\n"); */
    }
}

static JSValue native_gpio_on(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    int8_t ret      = -1;
    int8_t result   = -1;
    int8_t irq_edge = 0;
    item_handle_t gpio_handle;
    gpio_handle.handle      = NULL;
    gpio_dev_t *gpio_device = NULL;
    gpio_params_t *priv = NULL;
    const char *edge;
    gpio_irq_notify_param_t *notify = NULL;

    notify = aos_malloc(sizeof(gpio_irq_notify_param_t));
    aos_printf("func %p alloc memory %p \n", __func__, notify);
    if (!notify)
        goto out;

    gpio_handle.handle = JS_GetOpaque2(ctx, this_val, js_gpio_class_id);
    if (!gpio_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        aos_free(notify);
        goto out;
    }
    gpio_device        = board_get_node_by_handle(MODULE_GPIO, &gpio_handle);
    if (NULL == gpio_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        aos_free(notify);
        goto out;
    }

    priv = (gpio_params_t *)gpio_device->priv;
    priv->reserved = (void *)notify;  // 停止中断时，释放该内存
    irq_edge = priv->irq_mode;
    JSValue irq_cb = argv[0];
    if (!JS_IsFunction(ctx, irq_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    notify->js_cb_ref = JS_DupValue(ctx, irq_cb);
    notify->dev       = gpio_device;

    // amp_debug(MOD_STR, "%p, irq_edge:%04x port:%d", gpio_device, irq_edge, gpio_device->port);
    ret = aos_hal_gpio_enable_irq(gpio_device, irq_edge, gpio_irq, notify);
    if (ret < 0) {
        amp_error(MOD_STR, "aos_hal_gpio_enable_irq fail!");
        goto out;
    }

    result = 0;

out:
    return JS_NewInt32(ctx, result);
}

static JSClassDef js_gpio_class = {
    "GPIO",
};

static const JSCFunctionListEntry js_gpio_funcs[] = {
    JS_CFUNC_DEF("open", 1, native_gpio_open ),
    JS_CFUNC_DEF("read", 0, native_gpio_read ),
    JS_CFUNC_DEF("write", 1, native_gpio_write ),
    JS_CFUNC_DEF("toggle", 0, native_gpio_toggle),
    JS_CFUNC_DEF("on", 0, native_gpio_on),
    JS_CFUNC_DEF("close", 0, native_gpio_close ),
};

static int js_gpio_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_gpio_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_gpio_class_id, &js_gpio_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_gpio_funcs,
                               countof(js_gpio_funcs));
    JS_SetClassProto(ctx, js_gpio_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_gpio_funcs,
                                  countof(js_gpio_funcs));
}

JSModuleDef *js_init_module_gpio(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_gpio_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_gpio_funcs, countof(js_gpio_funcs));
    return m;
}

void module_gpio_register(void)
{
    amp_debug(MOD_STR, "module_gpio_register");
    JSContext *ctx = js_get_context();
    aos_printf("module gpio register\n");
    js_init_module_gpio(ctx, "GPIO");
}
