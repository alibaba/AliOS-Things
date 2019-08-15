/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "jse_common.h"
#include "be_inl.h"

#define MAGIC 0x55ff0055
typedef struct timer_wrap {
    int magic;
    int js_cb_ref;
    int repeat;
    void *timer_id;
} timer_wrap_t;

static void timer_action(void *arg)
{
    timer_wrap_t *t = (timer_wrap_t *)arg;

    /* Is the timer has been cleared? */
    if (t->magic != MAGIC) {
        jse_error("Timer has been cleared\n");
        return;
    }

    duk_context *ctx = be_get_context();
    be_push_ref(ctx, t->js_cb_ref);
    duk_pcall(ctx, 0);
    duk_pop(ctx);

    /* Is the timer has been cleared? */
    if (t->magic != MAGIC) {
        jse_error("Timer has been cleared\n");
        return;
    }

    if (!t->repeat) {
        be_unref(ctx, t->js_cb_ref);
        t->magic = 0;
        jse_free(t);
    }
}

static timer_wrap_t *setup_timer(int js_cb_ref, long ms, int repeat)
{
    timer_wrap_t *t = (timer_wrap_t *)jse_malloc(sizeof(*t));
    t->magic        = MAGIC;
    t->js_cb_ref    = js_cb_ref;
    t->repeat       = repeat;
    t->timer_id     = be_jse_task_timer_action(ms, timer_action, t, repeat);
    return t;
}

static void clear_timer(timer_wrap_t *t)
{
    if (!t) {
        jse_warn("timer wrap handle is null\n");
        return;
    }
    if (t->magic != MAGIC) {
        jse_warn("timer wrap handle has be cleared\n");
        return;
    }
    be_jse_task_cancel_timer(t->timer_id);
    duk_context *ctx = be_get_context();
    be_unref(ctx, t->js_cb_ref);
    t->magic = 0;
    jse_free(t);
}

static duk_ret_t native_setTimeout(duk_context *ctx)
{
    if (!(duk_is_function(ctx, 0) && duk_is_number(ctx, 1))) {
        jse_warn("invalid parameters\n");
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
        jse_warn("invalid parameters\n");
        duk_push_string(ctx,
                        "setInterval parameters must be function and number");
        return duk_throw(ctx);
    }
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

void module_timer_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_c_function(ctx, native_setTimeout, 2);
    duk_put_global_string(ctx, "setTimeout");

    duk_push_c_function(ctx, native_clearTimeout, 1);
    duk_put_global_string(ctx, "clearTimeout");

    duk_push_c_function(ctx, native_setInterval, 2);
    duk_put_global_string(ctx, "setInterval");

    duk_push_c_function(ctx, native_clearInterval, 1);
    duk_put_global_string(ctx, "clearInterval");
}
