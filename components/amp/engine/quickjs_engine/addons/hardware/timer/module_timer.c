/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "amp_task.h"
#include "aos_hal_timer.h"
#include "amp_list.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "HW_TIMER"

typedef struct {
    JSValue js_cb_ref;
    timer_dev_t *dev;
}timer_irq_notify_param_t;

typedef struct {
    timer_dev_t *dev;
    timer_irq_notify_param_t *notify;
    dlist_t node;
} timer_info_t;

static dlist_t g_timer_list = AMP_DLIST_HEAD_INIT(g_timer_list);
static JSClassID js_timer_class_id;

static void hw_timer_delete(timer_dev_t *dev)
{
    timer_info_t *timer_info;

    if (!dev)
        return;

    dlist_for_each_entry(&g_timer_list, timer_info, timer_info_t, node) {
        if (timer_info->dev == dev) {
            aos_hal_timer_stop(timer_info->dev);
            aos_hal_timer_finalize(timer_info->dev);
            dlist_del(&timer_info->node);
            aos_free(timer_info->notify);
            aos_free(timer_info);
            break;
        }
    }
}

static JSValue native_timer_open(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    item_handle_t timer_handle;
    timer_handle.handle     = NULL;
    timer_dev_t *timer_device = NULL;

    JSValue js_id = JS_GetPropertyStr(ctx, argv[0], "id");
    const char *id = JS_ToCString(ctx, js_id);

    if (id == NULL) {
        amp_error(MOD_STR, "get timer id fail!");
        goto out;
    }

    ret = board_attach_item(MODULE_TIMER, id, &timer_handle);
    if (0 != ret) {
        amp_error(MOD_STR, "board_attach_item fail!");
        goto out;
    }
    amp_debug(MOD_STR, "timer handle:%u", timer_handle.handle);
    timer_device = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    ret = aos_hal_timer_init(timer_device);
    if (0 != ret) {
        amp_error(MOD_STR, "aos_hal_timer_init fail!");
        goto out;
    }
out:
    if (id != NULL) {
        JS_FreeCString(ctx, id);
        JS_FreeValue(ctx, js_id);
    }

    if (0 != ret) {
        JS_SetContextOpaque(ctx, NULL);
        board_disattach_item(MODULE_TIMER, &timer_handle);
    } else {
        JSValue obj;
        obj = JS_NewObjectClass(ctx, js_timer_class_id);
        JS_SetOpaque(obj, (void *)timer_handle.handle);
        return obj;
    }
    return JS_NewInt32(ctx, 0);
}

static void native_timer_timeout_handler(void *args)
{
    timer_irq_notify_param_t *param = (timer_irq_notify_param_t *)args;
    JSContext *ctx = js_get_context();
    JSValue v = JS_NewInt32(ctx, 0);
    JSValue val = JS_Call(ctx, param->js_cb_ref, JS_UNDEFINED, 1, &v);
    JS_FreeValue(ctx, v);
    JS_FreeValue(ctx, val);
}

static void native_timer_timeout_cb(void *args)
{
    amp_task_schedule_call(native_timer_timeout_handler, args);
}

static JSValue native_timer_setTimeout(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;
    timer_info_t *timer_info;
    timer_irq_notify_param_t *notify;

    notify = aos_malloc(sizeof(timer_irq_notify_param_t));
    if (!notify) {
        goto out;
    }

    timer_info = aos_malloc(sizeof(timer_info_t));
    if (!timer_info) {
        amp_error(MOD_STR, "alloc timer info fail!");
        aos_free(notify);
        goto out;
    }

    timer_handle.handle = JS_GetOpaque2(ctx, this_val, js_timer_class_id);
    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        aos_free(notify);
        aos_free(timer_info);
        goto out;
    }

    JSValue irq_cb = argv[0];
    if (!JS_IsFunction(ctx, irq_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    JS_ToInt32(ctx, &timeout, argv[1]);

    notify->js_cb_ref = JS_DupValue(ctx, irq_cb);
    notify->dev       = timer_device;

    timer_device->config.reload_mode = TIMER_RELOAD_MANU;
    timer_device->config.period = timeout;
    timer_device->config.cb = native_timer_timeout_cb;
    timer_device->config.arg = notify;

    aos_hal_timer_stop(timer_device);
    ret = aos_hal_timer_start(timer_device);
    if (ret != 0) {
        amp_error(MOD_STR, "amp_hal_set_timeout fail!");
        aos_free(timer_info);
        goto out;
    }

    timer_info->dev    = timer_device;
    timer_info->notify = notify;
    dlist_add_tail(&timer_info->node, &g_timer_list);

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_timer_clearTimeout(JSContext *ctx, JSValueConst this_val,
                                         int argc, JSValueConst *argv)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;

    timer_handle.handle = JS_GetOpaque2(ctx, this_val, js_timer_class_id);
    if (!timer_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    timer_device->config.reload_mode = TIMER_RELOAD_MANU;

    aos_hal_timer_stop(timer_device);
    ret = 0;

out:
    return JS_NewInt32(ctx, ret);
}

static void native_timer_interval_handler(void *arg)
{
    timer_irq_notify_param_t *param = (timer_irq_notify_param_t *)arg;
    JSContext *ctx = js_get_context();
    JSValue value = JS_NewInt32(ctx, 0);
    JSValue val = JS_Call(ctx, param->js_cb_ref, JS_UNDEFINED, 1, &value);
    JS_FreeValue(ctx, value);
    JS_FreeValue(ctx, val);
}

static void native_timer_interval_cb(void *args)
{
    amp_task_schedule_call(native_timer_interval_handler, args);
}

static JSValue native_timer_setInterval(JSContext *ctx, JSValueConst this_val,
                                        int argc, JSValueConst *argv)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;
    timer_info_t *timer_info;
    timer_irq_notify_param_t *notify;

    notify = aos_malloc(sizeof(timer_irq_notify_param_t));
    if (!notify) {
        goto out;
    }

    timer_info = aos_malloc(sizeof(timer_info_t));
    if (!timer_info) {
        amp_error(MOD_STR, "alloc timer info fail!");
        aos_free(notify);
        goto out;
    }

    timer_handle.handle = JS_GetOpaque2(ctx, this_val, js_timer_class_id);
    if (!timer_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        aos_free(notify);
        aos_free(timer_info);
        goto out;
    }

    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        aos_free(notify);
        aos_free(timer_info);
        goto out;
    }

    JSValue irq_cb = argv[0];
    if (!JS_IsFunction(ctx, irq_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }

    JS_ToInt32(ctx, &timeout, argv[1]);

    notify->js_cb_ref = JS_DupValue(ctx, irq_cb);
    notify->dev       = timer_device;

    timer_device->config.reload_mode = TIMER_RELOAD_AUTO;
    timer_device->config.period = timeout;
    timer_device->config.cb = native_timer_interval_cb;
    timer_device->config.arg = notify;

    ret = aos_hal_timer_start(timer_device);
    if (ret != 0) {
        amp_error(MOD_STR, "aos_hal_timer_start fail!");
        aos_free(notify);
        aos_free(timer_info);
        goto out;
    }

    timer_info->dev    = timer_device;
    timer_info->notify = notify;
    dlist_add_tail(&timer_info->node, &g_timer_list);

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_timer_clearInterval(JSContext *ctx, JSValueConst this_val,
                                          int argc, JSValueConst *argv)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;

    timer_handle.handle = JS_GetOpaque2(ctx, this_val, js_timer_class_id);
    if (!timer_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    timer_device->config.reload_mode = TIMER_RELOAD_AUTO;

    aos_hal_timer_stop(timer_device);
    ret = 0;

out:
    return JS_NewInt32(ctx, ret);
}

static JSValue native_timer_close(JSContext *ctx, JSValueConst this_val,
                                  int argc, JSValueConst *argv)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;

    timer_handle.handle = JS_GetOpaque2(ctx, this_val, js_timer_class_id);
    if (!timer_handle.handle) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    hw_timer_delete(timer_device);
out:
    return JS_NewInt32(ctx, ret);
}

static void native_timer_source_free(void)
{
    dlist_t *temp;
    timer_info_t *timer_node;

    dlist_for_each_entry_safe(&g_timer_list, temp, timer_node, timer_info_t, node) {
        aos_hal_timer_stop(timer_node->dev);
        aos_hal_timer_finalize(timer_node->dev);
        dlist_del(&timer_node->node);
        aos_free(timer_node);
    }
}

static JSClassDef js_timer_class = {
    "TIMER",
};

static const JSCFunctionListEntry js_timer_funcs[] = {
    JS_CFUNC_DEF("open", 0,          native_timer_open),
    JS_CFUNC_DEF("setTimeout", 2,    native_timer_setTimeout),
    JS_CFUNC_DEF("clearTimeout", 0,  native_timer_clearTimeout),
    JS_CFUNC_DEF("setInterval", 2,   native_timer_setInterval),
    JS_CFUNC_DEF("clearInterval", 0, native_timer_clearInterval),
    JS_CFUNC_DEF("close", 0,         native_timer_close),
};

static int js_timer_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_timer_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_timer_class_id, &js_timer_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_timer_funcs,
                               countof(js_timer_funcs));
    JS_SetClassProto(ctx, js_timer_class_id, proto);

    return JS_SetModuleExportList(ctx, m, js_timer_funcs,
                                  countof(js_timer_funcs));
}

JSModuleDef *js_init_module_timer(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_timer_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_timer_funcs, countof(js_timer_funcs));
    return m;
}

void module_timer_register(void)
{
    amp_debug(MOD_STR, "module_timer_register");
    JSContext *ctx = js_get_context();

    amp_module_free_register(native_timer_source_free);

    js_init_module_timer(ctx, "timer");
}

