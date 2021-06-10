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
#include "amp_task.h"
#include "amp_list.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "SYSTIMER"
#define MAGIC 0x55ff0055
typedef struct timer_wrap {
    int magic;
    JSValue js_cb_ref;
    int repeat;
    aos_timer_t *timer_id;
} timer_wrap_t;

typedef struct {
    aos_timer_t *timer_id;
    timer_wrap_t *t;
    dlist_t node;
} timer_link_t;

static dlist_t g_systimer_list = AMP_DLIST_HEAD_INIT(g_systimer_list);
static JSClassID js_systimer_class_id;

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

static void timer_action(void *arg)
{
    timer_wrap_t *t = (timer_wrap_t *)arg;
    JSValue fun;

    /* Is the timer has been cleared? */
    if (t->magic != MAGIC) {
        // amp_error(MOD_STR, "Timer has been cleared");
        return;
    }

    JSContext *ctx = js_get_context();

    /* Note when clearInterval is called within the callback function */
    if (!t->repeat) {
        if (t->magic != MAGIC) {
            amp_warn(MOD_STR, "timer wrap handle has be cleared");
            return;
        }
        t->magic = 0;
        fun = t->js_cb_ref;
        cancel_timer(t);
        aos_free(t);
    } else {
        fun = JS_DupValue(ctx, t->js_cb_ref);
    }

    JSValue value = JS_NewInt32(ctx, 0);
    JSValue val = JS_Call(ctx, fun, JS_UNDEFINED, 1, &value);
    JS_FreeValue(ctx, value);
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, fun);
}

static timer_wrap_t *setup_timer(JSValue js_cb_ref, long ms, int repeat)
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
            timer_link->t        = t;
            dlist_add_tail(&timer_link->node, &g_systimer_list);
        }
    }
    return t;
}

static void clear_timer(timer_wrap_t *t)
{
    JSContext *ctx = js_get_context();

    if (!t) {
        amp_warn(MOD_STR, "timer wrap handle is null");
        return;
    }
    if (t->magic != MAGIC) {
        amp_warn(MOD_STR, "timer wrap handle has be cleared");
        return;
    }

    JS_FreeValue(ctx, t->js_cb_ref);
    cancel_timer(t->timer_id);
    t->magic = 0;
    aos_free(t);
}

static JSValue native_setTimeout(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    long ms;

    JSValue irq_cb = argv[0];
    if (!JS_IsFunction(ctx, irq_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }

    JSValue js_cb_ref = JS_DupValue(ctx, irq_cb);

    JS_ToInt32(ctx, &ms, argv[1]);

    timer_wrap_t *t = setup_timer(js_cb_ref, ms, 0);

    JSValue obj;
    obj = JS_NewObjectClass(ctx, js_systimer_class_id);
    JS_SetOpaque(obj, (void *)t);
    return obj;
}

static JSValue native_clearTimeout(JSContext *ctx, JSValueConst this_val,
                                   int argc, JSValueConst *argv)
{
    timer_wrap_t *t = JS_GetOpaque2(ctx, argv[0], js_systimer_class_id);
    clear_timer(t);
    return JS_NewInt32(ctx, 0);
}

static JSValue native_setInterval(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    long ms;

    JSValue irq_cb = argv[0];
    if (!JS_IsFunction(ctx, irq_cb)) {
        return JS_ThrowTypeError(ctx, "not a function");
    }

    JSValue js_cb_ref = JS_DupValue(ctx, irq_cb);

    JS_ToInt32(ctx, &ms, argv[1]);

    timer_wrap_t *t = setup_timer(js_cb_ref, ms, 1);

    JSValue obj;
    obj = JS_NewObjectClass(ctx, js_systimer_class_id);
    JS_SetOpaque(obj, (void *)t);
    return obj;
}

static JSValue native_clearInterval(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv)
{
    timer_wrap_t *t = JS_GetOpaque2(ctx, argv[0], js_systimer_class_id);
    clear_timer(t);
    return JS_NewInt32(ctx, 0);
}

static JSValue native_sleepMs(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv)
{
    int32_t ret = -1;
    uint32_t ms = 0;

    JS_ToInt32(ctx, &ms, argv[0]);

    amp_debug(MOD_STR, "system delay %d ms", ms);

    aos_msleep(ms);

    return JS_NewInt32(ctx, 0);

out:
    return JS_NewInt32(ctx, -1);
}

static void module_systimer_source_clean(void)
{
    dlist_t *temp;
    timer_link_t *timer_node;

    dlist_for_each_entry_safe(&g_systimer_list, temp, timer_node, timer_link_t, node)
    {
        clear_timer(timer_node->t);
    }
}

static JSClassDef js_systimer_class = {
    "SYSTIMER",
};

static int js_timer_init(JSContext *ctx)
{
    JSValue proto;

    JS_NewClassID(&js_systimer_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_systimer_class_id, &js_systimer_class);
    proto = JS_NewObject(ctx);
    JS_SetClassProto(ctx, js_systimer_class_id, proto);

    return 0;
}

void module_systimer_register(void)
{
    amp_debug(MOD_STR, "module_systimer_register");
    JSContext *ctx = js_get_context();

    amp_module_free_register(module_systimer_source_clean);

    js_timer_init(ctx);

    QUICKJS_GLOBAL_FUNC("setTimeout", native_setTimeout);
    QUICKJS_GLOBAL_FUNC("clearTimeout", native_clearTimeout);
    QUICKJS_GLOBAL_FUNC("setInterval", native_setInterval);
    QUICKJS_GLOBAL_FUNC("clearInterval", native_clearInterval);
    QUICKJS_GLOBAL_FUNC("sleepMs", native_sleepMs);
}

