/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "be_jse_task.h"
#include "be_log.h"
#include "be_port_osal.h"
#include "bone_engine_inl.h"

#define MAGIC 0x55ff0055
typedef struct timer_wrap {
    int magic;
    int js_cb_ref;
    int repeat;
    void *timer_id;
} timer_wrap_t;

/*
 * 在定时器清除过程中，底层又上报了定时消息到task的消息队列中，这时对
 * 应的timer_wrap_t handle已经释放,并且可能又分配出去了
 */
static void timer_action(void *arg) {
    debug("in\n");
    timer_wrap_t *t = (timer_wrap_t *)arg;

    /* 这里判断下，仅能减少出问题的概率 */
    if (t->magic != MAGIC) {
        error("Timer has been cleared\n");
        return;
    }

    duk_context *ctx = bone_engine_get_context();
    bone_engine_push_ref(ctx, t->js_cb_ref);
    duk_pcall(ctx, 0);
    duk_pop(ctx);

    /* 在js回调函数中清除自身 */
    if (t->magic != MAGIC) {
        error("Timer has been cleared\n");
        return;
    }

    if (!t->repeat) {
        bone_engine_unref(ctx, t->js_cb_ref);
        t->magic = 0;
        free(t);
    }
}

static timer_wrap_t *setup_timer(int js_cb_ref, long ms, int repeat) {
    debug("in\n");
    timer_wrap_t *t = (timer_wrap_t *)malloc(sizeof(*t));
    t->magic        = MAGIC;
    t->js_cb_ref    = js_cb_ref;
    t->repeat       = repeat;
    t->timer_id     = be_jse_task_timer_action(ms, timer_action, t, repeat);
    return t;
}

static void clear_timer(timer_wrap_t *t) {
    debug("in\n");
    if (!t) {
        warn("timer wrap handle is null\n");
        return;
    }
    if (t->magic != MAGIC) {
        warn("timer wrap handle has be cleared\n");
        return;
    }
    be_jse_task_cancel_timer(t->timer_id);
    duk_context *ctx = bone_engine_get_context();
    bone_engine_unref(ctx, t->js_cb_ref);
    t->magic = 0;
    free(t);
}

static duk_ret_t native_setTimeout(duk_context *ctx) {
    debug("in\n");
    if (!(duk_is_function(ctx, 0) && duk_is_number(ctx, 1))) {
        warn("invalid parameters\n");
        duk_push_string(ctx,
                        "setTimeout parameters must be function and number");
        return duk_throw(ctx);
    }
    duk_dup(ctx, 0);
    int js_cb_ref   = bone_engine_ref(ctx);
    long ms         = (long)duk_get_number(ctx, 1);
    timer_wrap_t *t = setup_timer(js_cb_ref, ms, 0);
    duk_push_pointer(ctx, t);
    return 1;
}

static duk_ret_t native_clearTimeout(duk_context *ctx) {
    debug("in\n");
    timer_wrap_t *t = (timer_wrap_t *)duk_get_pointer(ctx, 0);
    clear_timer(t);
    return 0;
}

static duk_ret_t native_setInterval(duk_context *ctx) {
    debug("in\n");
    if (!(duk_is_function(ctx, 0) && duk_is_number(ctx, 1))) {
        warn("invalid parameters\n");
        duk_push_string(ctx,
                        "setInterval parameters must be function and number");
        return duk_throw(ctx);
    }
    duk_dup(ctx, 0);
    int js_cb_ref   = bone_engine_ref(ctx);
    long ms         = (long)duk_get_number(ctx, 1);
    timer_wrap_t *t = setup_timer(js_cb_ref, ms, 1);
    duk_push_pointer(ctx, t);
    return 1;
}

static duk_ret_t native_clearInterval(duk_context *ctx) {
    debug("in\n");
    timer_wrap_t *t = (timer_wrap_t *)duk_get_pointer(ctx, 0);
    clear_timer(t);
    return 0;
}

void module_timer_register(void) {
    duk_context *ctx = bone_engine_get_context();

    duk_push_c_function(ctx, native_setTimeout, 2);
    duk_put_global_string(ctx, "setTimeout");

    duk_push_c_function(ctx, native_clearTimeout, 1);
    duk_put_global_string(ctx, "clearTimeout");

    duk_push_c_function(ctx, native_setInterval, 2);
    duk_put_global_string(ctx, "setInterval");

    duk_push_c_function(ctx, native_clearInterval, 1);
    duk_put_global_string(ctx, "clearInterval");
}
