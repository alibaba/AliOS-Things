/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "amp_task.h"
#include "amp_list.h"

#define MOD_STR "TIMER"
#define MAGIC 0x55ff0055
typedef struct timer_wrap {
    int magic;
    int js_cb_ref;
    int repeat;
    aos_timer_t *timer_id;
} timer_wrap_t;

typedef struct {
    aos_timer_t *timer_id;
    dlist_t node;
} timer_link_t;

static dlist_t g_systimer_list = AMP_DLIST_HEAD_INIT(g_systimer_list);

static void clear_timer(timer_wrap_t *t);

static void timer_action(void *arg)
{
    timer_wrap_t *t = (timer_wrap_t *)arg;

    /* Is the timer has been cleared? */
    if (t->magic != MAGIC) {
        // amp_error(MOD_STR, "Timer has been cleared");
        return;
    }

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, t->js_cb_ref);
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
        // amp_console("%s", duk_safe_to_string(ctx, -1));
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);

    if (!t->repeat) {
        be_unref(ctx, t->js_cb_ref);
        clear_timer(t);
    }
    duk_gc(ctx, 0);
}

static timer_wrap_t *setup_timer(int js_cb_ref, long ms, int repeat)
{
    timer_link_t *timer_link;
    timer_wrap_t *t = (timer_wrap_t *)aos_malloc(sizeof(*t));
    t->magic        = MAGIC;
    t->js_cb_ref    = js_cb_ref;
    t->repeat       = repeat;
    t->timer_id     = amp_task_timer_action(ms, timer_action, t, repeat);
    if (t->timer_id) {
        timer_link = aos_malloc(sizeof(timer_link_t));
        if (timer_link) {
            timer_link->timer_id = t->timer_id;
            dlist_add_tail(&timer_link->node, &g_systimer_list);
        }
    }
    return t;
}

static void cancel_timer(void *timerid)
{
    timer_link_t *timer_node;

    if (!timerid) {
        return;
    }

    dlist_for_each_entry(&g_systimer_list, timer_node, timer_link_t, node) {
        if (timer_node->timer_id == timerid) {
            aos_timer_stop((aos_timer_t *)timerid);
            aos_timer_free((aos_timer_t *)timerid);
            aos_free((void *)timerid);
            dlist_del(&timer_node->node);
            aos_free(timer_node);
            break;
        }
    }
}

static void clear_timer(timer_wrap_t *t)
{
    if (!t) {
        amp_warn(MOD_STR, "timer wrap handle is null");
        return;
    }
    if (t->magic != MAGIC) {
        amp_warn(MOD_STR, "timer wrap handle has be cleared");
        return;
    }
    cancel_timer(t->timer_id);
    duk_context *ctx = be_get_context();
    be_unref(ctx, t->js_cb_ref);
    t->magic = 0;
    aos_free(t);
}

static duk_ret_t native_setTimeout(duk_context *ctx)
{
    if (!(duk_is_function(ctx, 0) && duk_is_number(ctx, 1))) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "setTimeout parameters must be function and number");
        return duk_throw(ctx);
    }
    duk_dup(ctx, 0);
    int js_cb_ref   = be_ref(ctx);
    long ms         = (long)duk_get_number(ctx, 1);
    timer_wrap_t *t = setup_timer(js_cb_ref, ms, 0);
    duk_push_pointer(ctx, t);
    return 1;
}

static duk_ret_t native_clearTimeout(duk_context *ctx)
{
    timer_wrap_t *t = (timer_wrap_t *)duk_get_pointer(ctx, 0);
    clear_timer(t);
    return 0;
}

static duk_ret_t native_setInterval(duk_context *ctx)
{
    if (!(duk_is_function(ctx, 0) && duk_is_number(ctx, 1))) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "setInterval parameters must be function and number");
        return duk_throw(ctx);
    }
    amp_debug(MOD_STR, "native_setInterval");
    duk_dup(ctx, 0);
    int js_cb_ref   = be_ref(ctx);
    long ms         = (long)duk_get_number(ctx, 1);
    timer_wrap_t *t = setup_timer(js_cb_ref, ms, 1);
    duk_push_pointer(ctx, t);
    return 1;
}

static duk_ret_t native_clearInterval(duk_context *ctx)
{
    timer_wrap_t *t = (timer_wrap_t *)duk_get_pointer(ctx, 0);
    clear_timer(t);
    return 0;
}

static duk_ret_t native_sleepMs(duk_context *ctx)
{
    int32_t ret = -1;
    uint32_t ms = 0;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto out;
    }

    ms = duk_get_int(ctx, 0);
    amp_debug(MOD_STR, "system delay %d ms", ms);

    aos_msleep(ms);

    duk_push_int(ctx, 0);
    return 1;

out:
    duk_push_int(ctx, -1);
    return 1;
}

static void module_systimer_source_clean(void)
{
    dlist_t *temp;
    timer_link_t *timer_node;

    dlist_for_each_entry_safe(&g_systimer_list, temp, timer_node, timer_link_t, node)
    {
        aos_timer_stop((aos_timer_t *)timer_node->timer_id);
        aos_timer_free((aos_timer_t *)timer_node->timer_id);
        aos_free((void *)timer_node->timer_id);
        dlist_del(&timer_node->node);
        aos_free(timer_node);
    }
}

void module_systimer_register(void)
{
    amp_debug(MOD_STR, "module_timer_register");
    duk_context *ctx = be_get_context();

    amp_module_free_register(module_systimer_source_clean);

    AMP_ADD_GLOBAL_FUNCTION("setTimeout",    native_setTimeout, 2);
    AMP_ADD_GLOBAL_FUNCTION("clearTimeout",  native_clearTimeout, 1);
    AMP_ADD_GLOBAL_FUNCTION("setInterval",   native_setInterval, 2);
    AMP_ADD_GLOBAL_FUNCTION("clearInterval", native_clearInterval, 1);
    AMP_ADD_GLOBAL_FUNCTION("sleepMs",       native_sleepMs, 1);
}
