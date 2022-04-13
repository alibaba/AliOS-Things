/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_defines.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "amp_task.h"
#include "amp_hal_timer.h"
#include "infra_list.h"

#define MOD_STR "HW_TIMER"


typedef struct {
    timer_dev_t *dev;
    dlist_t node;
} timer_info_t;

static dlist_t g_timer_list = LIST_HEAD_INIT(g_timer_list);

void hw_timer_clear(void)
{
    dlist_t *temp;
    timer_info_t *timer_node;

    dlist_for_each_entry_safe(&g_timer_list, temp, timer_node, timer_info_t, node) {
        amp_hal_timer_stop(timer_node->dev);
        amp_hal_timer_finalize(timer_node->dev);
        dlist_del(&timer_node->node);
        amp_free(timer_node);
    }
}

static void hw_timer_delete(timer_dev_t *dev)
{
    timer_info_t *timer_info;

    if (!dev)
        return -1;

    dlist_for_each_entry(&g_timer_list, timer_info, timer_info_t, node) {
        if (timer_info->dev == dev) {
            amp_hal_timer_stop(timer_info->dev);
            amp_hal_timer_finalize(timer_info->dev);
            dlist_del(&timer_info->node);
            amp_free(timer_info);
            break;
        }
    }
}

static duk_ret_t native_open(duk_context *ctx)
{
    int32_t ret = -1;
    item_handle_t timer_handle;
    timer_handle.handle     = NULL;
    timer_dev_t *timer_device = NULL;

    if (!duk_is_string(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be string");
        goto out;
    }
    const char *id = duk_get_string(ctx, 0);
    ret            = board_attach_item(MODULE_TIMER, id, &timer_handle);
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
    ret = amp_hal_timer_init(timer_device);
    if (0 != ret) {
        amp_error(MOD_STR, "amp_hal_timer_init fail!");
        goto out;
    }
out:
    if (0 != ret) {
        duk_push_pointer(ctx, NULL);
        board_disattach_item(MODULE_TIMER, &timer_handle);
    } else {
        duk_push_pointer(ctx, (void *)timer_handle.handle);
    }
    return 1;
}

static void set_timeout_handler(void *args)
{
    int js_cb_ref = (int *)args;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, js_cb_ref);
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
}

static duk_ret_t native_setTimeout(duk_context *ctx)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;
    int js_cb_ref;
    timer_info_t *timer_info;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1) || 
        !duk_is_number(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be handle function and number");
        goto out;
    }

    timer_info = amp_malloc(sizeof(timer_info_t));
    if (!timer_info) {
        amp_error(MOD_STR, "alloc timer info fail!");
        goto out;
    }

    timer_handle.handle = duk_get_pointer(ctx, 0);
    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        amp_free(timer_info);
        goto out;
    }

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    timeout = duk_get_number(ctx, 2);

    timer_device->config.reload_mode = TIMER_RELOAD_MANU;
    timer_device->config.period = timeout;
    timer_device->config.cb = set_timeout_handler;
    timer_device->config.arg = js_cb_ref;

    amp_hal_timer_stop(timer_device);
    ret = amp_hal_timer_start(timer_device);
    if (ret != 0) {
        amp_error(MOD_STR, "amp_hal_set_timeout fail!");
        amp_free(timer_info);
        goto out;
    }

    timer_info->dev = timer_device;
    dlist_add_tail(&timer_info->node, &g_timer_list);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_clearTimeout(duk_context *ctx)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    timer_handle.handle = duk_get_pointer(ctx, 0);
    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    timer_device->config.reload_mode = TIMER_RELOAD_MANU;

    amp_hal_timer_stop(timer_device);
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void native_timer_interval_handler(void *arg)
{
    int js_cb_ref = (int *)arg;

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, js_cb_ref);
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    duk_gc(ctx, 0);
}

static void native_timer_interval_cb(void *args)
{
    amp_task_schedule_call(native_timer_interval_handler, args);
}

static duk_ret_t native_setInterval(duk_context *ctx)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;
    int js_cb_ref;
    timer_info_t *timer_info;

    if (!duk_is_pointer(ctx, 0) || !duk_is_function(ctx, 1) || 
        !duk_is_number(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be handle function and number");
        goto out;
    }

    timer_info = amp_malloc(sizeof(timer_info_t));
    if (!timer_info) {
        amp_error(MOD_STR, "alloc timer info fail!");
        goto out;
    }

    timer_handle.handle = duk_get_pointer(ctx, 0);
    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        amp_free(timer_info);
        goto out;
    }

    duk_dup(ctx, 1);
    js_cb_ref = be_ref(ctx);

    timeout = duk_get_number(ctx, 2);

    timer_device->config.reload_mode = TIMER_RELOAD_AUTO;
    timer_device->config.period = timeout;
    timer_device->config.cb = native_timer_interval_cb;
    timer_device->config.arg = js_cb_ref;

    ret = amp_hal_timer_start(timer_device);
    if (ret != 0) {
        amp_error(MOD_STR, "amp_hal_timer_start fail!");
        amp_free(timer_info);
        goto out;
    }

    timer_info->dev = timer_device;
    dlist_add_tail(&timer_info->node, &g_timer_list);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_clearInterval(duk_context *ctx)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    timer_handle.handle = duk_get_pointer(ctx, 0);
    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    timer_device->config.reload_mode = TIMER_RELOAD_AUTO;

    amp_hal_timer_stop(timer_device);
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_close(duk_context *ctx)
{
    int32_t ret    = -1;
    uint32_t timeout = 0;
    item_handle_t timer_handle;
    timer_dev_t *timer_device = NULL;

    if (!duk_is_pointer(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be handle");
        goto out;
    }

    timer_handle.handle = duk_get_pointer(ctx, 0);
    timer_device        = board_get_node_by_handle(MODULE_TIMER, &timer_handle);
    if (NULL == timer_device) {
        amp_error(MOD_STR, "board_get_node_by_handle fail!");
        goto out;
    }

    amp_hal_timer_stop(timer_device);

    ret = amp_hal_timer_finalize(timer_device);
    if (ret != 0) {
        amp_error(MOD_STR, "amp_hal_timer_finalize fail!");
        goto out;
    }  
    hw_timer_delete(timer_device);
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_timer_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_setTimeout, 3);
    duk_put_prop_string(ctx, -2, "setTimeout");

    duk_push_c_function(ctx, native_clearTimeout, 1);
    duk_put_prop_string(ctx, -2, "clearTimeout");

    duk_push_c_function(ctx, native_setInterval, 3);
    duk_put_prop_string(ctx, -2, "setInterval");

    duk_push_c_function(ctx, native_clearInterval, 1);
    duk_put_prop_string(ctx, -2, "clearInterval");

    duk_push_c_function(ctx, native_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_prop_string(ctx, -2, "TIMER");
}
